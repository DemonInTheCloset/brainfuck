#include "bfclean.h"
#include "bfprint.h"
#include <getopt.h>
#include <stdbool.h>

static const char usage[] = "usage: %s [-s] FILE"
                            "\n\nRequired:"
                            "\n\tFILE: Path to the Brainfuck program"
                            "\n\nOptional:"
                            "\n\t-s:   Interpret FILE as a Brainfuck program"
                            "\n";

int main(int argc, char **argv) {
  bool is_file = true;
  int opt;

  while ((opt = getopt(argc, argv, "hs")) != -1) {
    switch (opt) {
    case 'h':
      printf(usage, argv[0]);
      return EXIT_SUCCESS;
    case 's':
      is_file = false;
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

  printf("Is file: %d\n", is_file);
  printf("FILE: \"%s\"\n", argv[optind]);
  return EXIT_SUCCESS;
}
