#include "bfclean.h"

static inline uint8_t to_instr(char c) {
  switch (c) {
  case '+':
    return (uint8_t)((1u << 2) | BF_ADD);
  case '-':
    return (uint8_t)((-1u << 2) | BF_ADD);
  case '>':
    return (uint8_t)((1u << 2) | BF_PTR);
  case '<':
    return (uint8_t)((-1u << 2) | BF_PTR);
  case '[':
    return (uint8_t)((0u << 2) | BF_JMP);
  case ']':
    return (uint8_t)((-1u << 2) | BF_JMP);
  case '.':
    return (uint8_t)((0u << 2) | BF_IOC);
  case ',':
    return (uint8_t)((-1u << 2) | BF_IOC);
  default:
    return 0;
  }
}

size_t remove_comments(size_t n, const char prog[n], uint8_t output[n]) {
  size_t ic = 0;

  for (size_t i = 0; i < n; ++i) {
    switch (prog[i]) {
    case '[':
      // Comment: skip
      if (ic == 0) {
        int count = 0, done = 0;
        for (++i; i < n && !done; ++i) {
          switch (prog[i]) {
          case '[':
            count++;
            break;
          case ']':
            done = count-- == 0;
            break;
          }
        }
      }
      __attribute__((fallthrough));
    case '+':
    case '-':
    case '>':
    case '<':
    case ']':
    case '.':
    case ',':
      // printf("char: %c, instr: 0x%02x\n", prog[i], to_instr(prog[i]));
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
    // printf("split : 0x%02x -> 0x%x % 4d\n", prog[i], opt, val);

    if (opt == last) {
      switch (instr_opt(prog[i])) {
      case BF_ADD:
      case BF_PTR:
        value += val;
        break;
      case BF_JMP:
      case BF_IOC:
        // printf("merged: 0x%02x <- 0x%x % 4d\n", instr(value, last), last,
        // value);
        output[ic++] = instr(value, last);

        value = val;
        last = opt;
        break;
      }
    } else {
      // printf("merged: 0x%02x <- 0x%x % 4d\n", instr(value, last), last,
      // value);
      output[ic++] = instr(value, last);

      value = val;
      last = opt;
    }
  }

  // printf("merged: 0x%02x <- 0x%x % 4d\n", instr(value, last), last, value);
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
      __attribute__((fallthrough));
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
    memcpy(prog, output, n); // NOLINT
    return new_n;
  }

  n = new_n;

  // Recurse
  return simplify(n, prog, output);
}

int match_jumps(size_t n, uint8_t prog[n]) {
  if (n == 0)
    return 0;

  size_t *stack = malloc(sizeof(size_t) * n);
  size_t sp = 0;

  if (!stack) {
    fprintf(stderr, "Error: Failed to allocate %luB stack\n",
            sizeof(size_t) * n);
    return 0;
  }

  for (size_t ic = 0; ic < n; ++ic) {
    BF_OPT opt = instr_opt(prog[ic]);

    switch (opt) {
    case BF_IOC:
    case BF_PTR:
    case BF_ADD:
      break;
    case BF_JMP:
      if (instr_imm(prog[ic])) {
        if (!sp--) {
          free(stack);
          fprintf(stderr, "Error: unmatched ']'\n");
          return 1;
        }

        size_t jump = ic - stack[sp];
        if (jump >= (1u << 13)) {
          fprintf(stderr, "Warning: jump too long (%lu >= %u)\n", jump,
                  (1u << 13));
        } else {
          int forward = jump + 1;
          int backward = 1 - (int)jump;

          prog[ic] = instr(backward, BF_JMP);
          prog[stack[sp]] = instr(forward, BF_JMP);
        }
      } else {
        stack[sp++] = ic;
      }
      break;
    }
  }

  free(stack);
  return 0;
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

  if (match_jumps(n, output)) {
    return 0;
  }

  return n;
}
