#include "bfclean.h"

static inline uint8_t to_instr(char c) {
  switch (c) {
  case '+':
    return (uint8_t)(1u << 2) | BF_ADD;
  case '-':
    return (uint8_t)(-1u << 2) | BF_ADD;
  case '>':
    return (uint8_t)(1u << 2) | BF_PTR;
  case '<':
    return (uint8_t)(-1u << 2) | BF_PTR;
  case '[':
    return (uint8_t)(0u << 2) | BF_JMP;
  case ']':
    return (uint8_t)(-1u << 2) | BF_JMP;
  case '.':
    return (uint8_t)(0u << 2) | BF_IOC;
  case ',':
    return (uint8_t)(-1u << 2) | BF_IOC;
  default:
    return 0;
  }
}

static inline uint8_t instr(int val, BF_OPT opt) { return (val << 2) | opt; }

size_t remove_comments(size_t n, const char prog[n], uint8_t output[n]) {
  size_t ic = 0;

  for (size_t i = 0; i < n; ++i) {
    switch (prog[i]) {
    case '+':
    case '-':
    case '>':
    case '<':
    case '[':
    case ']':
      output[ic++] = to_instr(prog[i]);
      break;
    default:
      break;
    }
  }

  return ic;
}

size_t merge_same(size_t n, const uint8_t prog[n], uint8_t output[n]) {
  if (n == 0)
    return 0;

  size_t ic = 0;
  int value = instr_imm(prog[0]);
  BF_OPT last = instr_opt(prog[0]);

  for (size_t i = 1; i < n; ++i) {
    int val = instr_imm(prog[i]);
    BF_OPT opt = instr_opt(prog[i]);

    if (opt == last) {
      switch (instr_opt(prog[i])) {
      case BF_ADD:
      case BF_PTR:
        value += val;
        break;
      case BF_JMP:
      case BF_IOC:
        output[ic++] = instr(value, last);
        value = val;
        last = val;
        break;
      }
    } else {
      output[ic++] = instr(value, last);
      value = val;
      last = opt;
    }
  }

  output[ic++] = instr(value, last);

  return ic;
}

size_t remove_redundant(size_t n, const uint8_t prog[n], uint8_t output[n]) {
  if (n == 0)
    return 0;

  size_t ic = 0;
  for (size_t i = 0; i < n; ++i) {
    switch (instr_opt(prog[0])) {
    case BF_ADD:
    case BF_PTR:
      if (instr_imm(prog[0]) == 0) {
        break;
      }
    case BF_JMP:
    case BF_IOC:
      output[ic++] = prog[i];
      break;
    }
  }

  return ic;
}

size_t simplify(size_t n, uint8_t prog[n], uint8_t output[n]) {
  size_t new_n = merge_same(n, prog, output);

  // Done
  if (new_n == n)
    return new_n;

  n = new_n;
  new_n = remove_redundant(n, output, prog);

  // Done
  if (new_n == n) {
    memcpy(prog, output, n);
    return new_n;
  }

  n = new_n;

  // Recurse
  return simplify(n, prog, output);
}

size_t proccess(size_t n, const char prog[n], uint8_t output[n]) {
  uint8_t *buffer = malloc(sizeof(uint8_t) * n);

  if (!buffer) {
    fprintf(stderr, "Failed to allocate %luB buffer\n", sizeof(uint8_t) * n);
    return 0;
  }

  n = remove_comments(n, prog, buffer);
  n = simplify(n, buffer, output);

  free(buffer);

  return n;
}
