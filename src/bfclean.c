#include "bfclean.h"

BFProgram *make_program(uint64_t length) {
  BFProgram *prog = malloc(sizeof(BFProgram) + length * sizeof(uint8_t));

  if (prog) {
    prog->length = length;
    prog->required = 0;
  }

  return prog;
}

bool valid(const char *program, uint64_t length) {
  int64_t count = 0;

  for (uint64_t pc = 0; pc != length;) {
    switch (program[pc++]) {
    case '[':
      ++count;
      break;
    case ']':
      --count;
      break;
    default:
      break;
    }
  }

  return count == 0;
}

uint64_t number_of_instr(const char *program, uint64_t prog_length) {
  uint64_t length = 0;

  for (uint64_t pc = 0; pc != prog_length;) {
    switch (program[pc++]) {
    case '+':
    case '-':
      ++length;
      {
        bool running = true;
        while (pc != prog_length && running) {
          switch (program[pc++]) {
          case '.':
          case ',':
          case '>':
          case '<':
          case '[':
          case ']':
            --pc;
            running = false;
            break;
          case '+':
          case '-':
          default:
            break;
          }
        }
      }
      break;
    case '>':
    case '<':
      ++length;
      {
        bool running = true;
        while (pc != prog_length && running) {
          switch (program[pc++]) {
          case '.':
          case ',':
          case '+':
          case '-':
          case '[':
          case ']':
            --pc;
            running = false;
            break;
          case '>':
          case '<':
          default:
            break;
          }
        }
      }
      break;
    case '[':
    case ']':
    case '.':
    case ',':
      ++length;
      break;
    default:
      break;
    }
  }
  return length;
}

BFProgram *cleanup(const char *program, uint64_t prog_length) {
  BFProgram *prog_src = make_program(number_of_instr(program, prog_length));

  if (!prog_src) {
    return NULL;
  }

  {
    uint64_t ic = 0;
    int64_t value = 0;
    for (uint64_t pc = 0; pc != prog_length;) {
      switch (program[pc++]) {
      case '+':
        ++value;
        if (program[pc] != '+' && program[pc] != '-') {
          prog_src->program[ic++] = ((value & 0x3f) << 2) | BF_ADD_CELL;
          value = 0;
        }
        break;
      case '-':
        --value;
        if (program[pc] != '+' && program[pc] != '-') {
          prog_src->program[ic++] = ((value & 0x3f) << 2) | BF_ADD_CELL;
          value = 0;
        }
        break;
      case '>':
        ++value;
        if (program[pc] != '>' && program[pc] != '<') {
          prog_src->program[ic++] = ((value & 0x3f) << 2) | BF_ADD_PTR;
          value = 0;
        }
        break;
      case '<':
        --value;
        if (program[pc] != '>' && program[pc] != '<') {
          prog_src->program[ic++] = ((value & 0x3f) << 2) | BF_ADD_PTR;
          value = 0;
        }
        break;
      case '[':
        prog_src->program[ic++] = 0x00 | BF_JMP;
        break;
      case ']':
        prog_src->program[ic++] = 0xfc | BF_JMP;
        break;
      case '.':
        prog_src->program[ic++] = 0x00 | BF_IO;
        break;
      case ',':
        prog_src->program[ic++] = 0xfc | BF_IO;
        break;
      }
    }
  }

  return prog_src;
}
