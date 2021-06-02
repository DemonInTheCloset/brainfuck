#include "bfas.h"
#include "bfclean.h"
#include "bfdef.h"
#include "bfinterpret.h"
#include "bfprint.h"
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

struct prog_args {
  char *src;
  char *dst;
  int verbose;
  bool is_file;
  bool to_byte_code;
  bool run_vm;
};

struct prog_args get_args(int argc, char **argv) {
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
      exit(EXIT_SUCCESS);
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
      exit(EXIT_FAILURE);
    default:
      fprintf(stderr, "Unhandled option: %c\n", optopt);
      fprintf(stderr, usage, argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if (optind != argc - 1) {
    fprintf(stderr, "Missing required argument: FILE\n");
    fprintf(stderr, usage, argv[0]);
    exit(EXIT_FAILURE);
  }

  return (struct prog_args){.src = argv[optind],
                            .dst = output,
                            .verbose = verbose,
                            .is_file = is_file,
                            .to_byte_code = byte_code,
                            .run_vm = run_vm};
}

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

int bf_vm(struct prog_args args) {
  if (!args.is_file) {
    fprintf(stderr, "Error: can only bytecode (.bfc)\n");
    return EXIT_FAILURE;
  }

  uint8_t *prog;
  BFC_HEADER header = read_bytecode(args.src, &prog);

  if (args.verbose) {
    print_header(header);
  }

  if (prog) {
    int success = EXIT_FAILURE;
    if (header.size) {
      success = interpret(header.size, prog, header.flags);

      if (args.verbose > 1) {
        pretty_print(header.size, prog, "  ");
      }
    }

    free(prog);
    return success;
  }

  return EXIT_FAILURE;
}

int bfbc_compiler(struct prog_args args) {
  char *prog = NULL;
  size_t len;

  // Get source string
  if (args.is_file) {
    len = read_file(args.src, &prog);
  } else {
    prog = args.src;
    len = strlen(prog);
  }

  if (!len) {
    if (args.is_file)
      free(prog);

    printf("Program of length 0 given; nothing to do\n");
    return EXIT_SUCCESS;
  }

  uint8_t *byte_code = malloc(sizeof(uint8_t) * len);

  if (!byte_code) {
    if (args.is_file)
      free(prog);

    fprintf(stderr, "Failed to allocate %luB buffer\n", sizeof(uint8_t) * len);
    return EXIT_FAILURE;
  }

  // Process source file into bytecode
  len = proccess(len, prog, byte_code);

  if (args.is_file)
    free(prog);

  if (!args.dst)
    args.dst = "a.bfc";

  if (args.verbose)
    pretty_print(len, byte_code, "  ");

  // Write byte_code to output file
  int success = write_bytecode(len, byte_code, BFC_RJMP, args.dst);

  free(byte_code);

  return success;
}

int bf_assemble(struct prog_args args) {
  uint8_t *prog = NULL;
  size_t len;

  // Get source string
  if (!args.is_file) {
    fprintf(stderr, "Error: can only assemble bytecode (.bfc)\n");
    return EXIT_FAILURE;
  }

  BFC_HEADER header = read_bytecode(args.src, &prog);

  if (args.verbose) {
    print_header(header);
  }

  if (!prog)
    return EXIT_FAILURE;

  if (!args.dst)
    args.dst = "a.S";

  char *out = assembler(header.size, prog);
  if (!out)
    return EXIT_FAILURE;

  FILE *dst_file = fopen(args.dst, "w");

  if (!dst_file) {
    fprintf(stderr, "Error: failed to open file %s", args.dst);
    return EXIT_FAILURE;
  }

  fputs(out, dst_file);

  return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
  struct prog_args args = get_args(argc, argv);

  if (args.run_vm) {
    return bf_vm(args);
  }

  if (args.to_byte_code) {
    return bfbc_compiler(args);
  }

  return bf_assemble(args);
}
