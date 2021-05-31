#pragma once
#include <stddef.h>
#include <stdint.h>

#define BFC_VER_MAJOR 1
#define BFC_VER_MINOR 0

typedef enum { BF_ADD = 0x0, BF_PTR = 0x1, BF_JMP = 0x2, BF_IOC = 0x3 } BF_OPT;

typedef enum { BFC_RJMP = 0x1 } BFC_FLAG;

typedef struct {
  uint16_t ver_major;
  uint16_t ver_minor;
  uint32_t flags;
  uint64_t size;
} BFC_HEADER;

static inline BF_OPT instr_opt(uint8_t instr) { return instr & 0x3; }

static inline int instr_imm(int8_t instr) { return instr >> 2; }

static inline uint8_t instr(unsigned val, BF_OPT opt) {
  return (val << 2) | opt;
}
