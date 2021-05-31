#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { BF_ADD = 0x0, BF_PTR = 0x1, BF_JMP = 0x2, BF_IOC = 0x3 } BF_OPT;

static inline BF_OPT instr_opt(uint8_t instr) { return instr & 0x3; }

static inline int instr_imm(int8_t instr) { return instr >> 2; }

size_t proccess(size_t n, const char prog[n], uint8_t output[n]);
