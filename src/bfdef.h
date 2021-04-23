#pragma once
#include <stdint.h>

enum BFInstruction {
  BF_ADD_CELL = 0x0,
  BF_ADD_PTR = 0x1,
  BF_JMP = 0x2,
  BF_IO = 0x3,
};

typedef enum BFInstruction BFInstruction;

struct BFProgram {
  uint64_t length;
  uint64_t required;
  uint8_t program[];
};

typedef struct BFProgram BFProgram;
