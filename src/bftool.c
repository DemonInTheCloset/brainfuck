#include "bfclean.h"
#include "bfdef.h"
#include "bfinterpret.h"
#include "bfprint.h"
#include <getopt.h>
#include <stdbool.h>

static const char usage[] =
    "usage: %s [-c] [-h] [-i] [-o OUTPUT] [-s] [-v]* FILE"
    "\n\nRequired:"
    "\n\tFILE:      Path to the Brainfuck program"
    "\n\nOptional:"
    "\n\t-c:        Compile to bytecode"
    "\n\t-h:        Print this help message"
    "\n\t-i:        Run the interpreter on FILE"
    "\n\t-o OUTPUT: Save to OUTPUT (default a.out)"
    "\n\t-s:        Interpret FILE as a Brainfuck program"
    "\n\t-v:        Increase verbose level"
    "\n";

size_t read_file(const char *path, char **output) {
  // Adapted from:
  // https://stackoverflow.com/questions/174531/how-to-read-the-content-of-a-file-to-a-string-in-c
  size_t n = 0;
  char *buffer = NULL;
  FILE *file = fopen(path, "r");

  if (!file) {
    fprintf(stderr, "Error: couldn't open \"%s\"\n", path);
  }

  if (fseek(file, 0, SEEK_END)) {
    fprintf(stderr, "Error: Failed to seek to EOF\n");
    fclose(file);
    return 0;
  }
  n = ftell(file);
  if (fseek(file, 0, SEEK_SET)) {
    fprintf(stderr, "Error: Failed to seek to start of file\n");
    fclose(file);
    return 0;
  }

  buffer = malloc(n);
  if (!buffer) {
    fprintf(stderr, "Error: Failed to allocate %luB buffer\n", n);
    fclose(file);
    return 0;
  }

  size_t read = fread(buffer, 1, n, file);
  fclose(file);
  if (read != n) {
    fprintf(stderr, "Warning: Read %luB/%luB\n", read, n);
    n = read;
  }

  *output = buffer;

  return n;
}

int main(int argc, char **argv) {
  bool is_file = true, byte_code = false, run_vm = false;
  char *output = NULL;
  int verbose = 0;
  int opt;

  while ((opt = getopt(argc, argv, "chio:sv")) != -1) {
    switch (opt) {
    case 'c':
      byte_code = true;
      break;
    case 'h':
      printf(usage, argv[0]);
      return EXIT_SUCCESS;
    case 'i':
      run_vm = true;
      break;
    case 'o':
      output = optarg;
      break;
    case 's':
      is_file = false;
      break;
    case 'v':
      ++verbose;
      break;
    case '?':
      fprintf(stderr, "Unkown option: %c\n", optopt);
      fprintf(stderr, usage, argv[0]);
      return EXIT_FAILURE;
    default:
      fprintf(stderr, "Unhandled option: %c\n", optopt);
      fprintf(stderr, usage, argv[0]);
      return EXIT_FAILURE;
    }
  }

  if (optind != argc - 1) {
    fprintf(stderr, "Missing required argument: FILE\n");
    fprintf(stderr, usage, argv[0]);
    return EXIT_FAILURE;
  }

  char *file = argv[optind];

  if (run_vm) {
    if (!is_file) {
      fprintf(stderr, "Error: can only interpret files\n");
      return EXIT_FAILURE;
    }

    uint8_t *prog;
    BFC_HEADER header = read_bytecode(file, &prog);

    if (prog) {
      int success = EXIT_FAILURE;
      if (header.size) {
        success = interpret(header.size, prog, header.flags);
      }

      free(prog);
      return success;
    }

    return EXIT_FAILURE;
  }

  char *text = NULL;
  size_t len = 0;

  if (is_file) {
    len = read_file(file, &text);
  } else {
    text = file;
    len = strlen(file);
  }

  if (!len) {
    printf("Program of length 0 given; nothing to do\n");
    return EXIT_SUCCESS;
  }

  uint8_t *buffer = malloc(sizeof(uint8_t) * len);

  if (!buffer) {
    fprintf(stderr, "Failed to allocate %luB buffer\n", sizeof(uint8_t) * len);
    return EXIT_FAILURE;
  }

  len = proccess(len, text, buffer);

  if (is_file) {
    free(text);
  }

  if (!output) {
    output = byte_code ? "a.bfc" : "a.out";
  }

  int success = write_bytecode(len, buffer, BFC_RJMP, output);

  if (verbose) {
    pretty_print(len, buffer, "  ");
  }

  free(buffer);

  return success;
}
