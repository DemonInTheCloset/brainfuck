#include "bfprint.h"

void print_prog(const BFProgram *prog) {
  for (uint64_t pc = 0; pc != prog->length;) {
    uint8_t instr = prog->program[pc++];
    int8_t value = instr >> 2;
    instr &= 0x3;

    switch (instr) {
    case BF_ADD_CELL: {
      char c;

      if (value & 0x20) { // sign extend
        value |= 0xa0;
        value = -value;
        c = '-';
      } else {
        c = '+';
      }

      for (int i = 0; i < value; ++i) {
        printf("%c", c);
      }
    } break;
    case BF_ADD_PTR: {
      char c;

      if (value & 0x20) { // sign extend
        value |= 0xa0;
        value = -value;
        c = '<';
      } else {
        c = '>';
      }

      for (int i = 0; i < value; ++i) {
        printf("%c", c);
      }

    } break;
    case BF_JMP: {
      if (value) {
        printf("]");
      } else {
        printf("[");
      }
    } break;
    case BF_IO: {
      if (value) {
        printf(",");
      } else {
        printf(".");
      }
    } break;
    }
  }
}
