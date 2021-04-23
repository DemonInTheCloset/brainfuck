#include "bfclean.h"
#include "bfprint.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc == 2) {
    printf("Program: \"%s\"\n", argv[1]);

    uint64_t length = strlen(argv[1]);

    if (!valid(argv[1], length)) {
      puts("Program is invalid: bracket mismatch");
      return 1;
    }

    BFProgram *prog = cleanup(argv[1], length);

    if (!prog) {
      puts("Couldn't allocate memory");
      return 1;
    }

    printf("Cleaned: \"");
    print_prog(prog);
    puts("\"");
    return 0;
  }

  return 1;
}
