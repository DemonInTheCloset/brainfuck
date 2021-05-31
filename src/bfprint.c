#include "bfprint.h"

static const char *instructions[] = {"add", "ptr", "jmp", "ioc"};
static const char *jumps[] = {"forward", "backward"};
static const char *iocalls[] = {"putc", "getc"};

void pretty_print(size_t n, uint8_t prog[n], const char *indent) {
  int indent_level = 0;

  for (size_t i = 0; i < n; ++i) {
    for (int j = 0; j < indent_level; ++j) {
      printf("%s", indent);
    }

    int value = instr_imm(prog[i]);
    BF_OPT opt = instr_opt(prog[i]);

    const char *name = instructions[opt];
    printf("%s ", name);

    switch (opt) {
    case BF_ADD:
    case BF_PTR:
      printf("%d\n", value);
      break;
    case BF_JMP:
      indent_level += (value < 0) ? -1 : 1;
      if (value == 0 || value == -1) {
        printf("%s\n", jumps[value == -1]);
      } else {
        printf("%d (line %lu)\n", value, i + value + 1);
      }
      break;
    case BF_IOC:
      if (value == 0 || value == -1) {
        printf("%s\n", iocalls[value == -1]);
      } else {
        printf("bad value: %d\n", value);
      }
      break;
    }
  }
}
