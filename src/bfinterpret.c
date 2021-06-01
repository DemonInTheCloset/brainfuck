#include "bfinterpret.h"

BFC_HEADER read_bytecode(const char *path, uint8_t **prog) {
  BFC_HEADER header = {
      .ver_year = 0, .ver_month = 0, .ver_day = 0, .flags = 0, .size = 0};
  *prog = NULL;

  FILE *file = fopen(path, "rb");

  if (!file) {
    fprintf(stderr, "Error: Failed to open file %s\n", path);
    return header;
  }

  if (1 != fread(&header, sizeof(header), 1, file)) {
    fprintf(stderr, "Error: Failed to read header\n");

    header = (BFC_HEADER){
        .ver_year = 0, .ver_month = 0, .ver_day = 0, .flags = 0, .size = 0};

    fclose(file);
    return header;
  }

  if (BFC_VER_YEAR != header.ver_year || BFC_VER_MONTH != header.ver_month ||
      BFC_VER_DAY != header.ver_day) {
    fprintf(stderr, "Error: expected version %d.%d.%d got %d.%d.%d\n",
            BFC_VER_YEAR, BFC_VER_MONTH, BFC_VER_DAY, header.ver_year,
            header.ver_month, header.ver_day);

    header = (BFC_HEADER){
        .ver_year = 0, .ver_month = 0, .ver_day = 0, .flags = 0, .size = 0};

    fclose(file);
    return header;
  }

  *prog = malloc(sizeof(uint8_t) * header.size);

  size_t read;
  if (header.size !=
      (read = fread(*prog, sizeof(uint8_t), header.size, file))) {
    fprintf(stderr, "Warning: read %luB/%luB\n", read, header.size);
    header.size = read;
  }

  fclose(file);
  return header;
}

int write_bytecode(size_t n, const uint8_t prog[n], uint32_t flags,
                   const char *path) {
  BFC_HEADER header = {.ver_year = BFC_VER_YEAR,
                       .ver_month = BFC_VER_MONTH,
                       .ver_day = BFC_VER_DAY,
                       .flags = flags,
                       .size = n};

  FILE *file = fopen(path, "wb");

  if (!file) {
    fprintf(stderr, "Error: couldn't open file %s\n", path);
    return EXIT_FAILURE;
  }

  if (1 != fwrite(&header, sizeof(header), 1, file)) {
    fprintf(stderr, "Error: failed to write header\n");

    fclose(file);
    return EXIT_FAILURE;
  }

  size_t wrote;
  if (n != (wrote = fwrite(prog, sizeof(uint8_t), n, file))) {
    fprintf(stderr, "Error: failed to write bytecode (%luB/%luB)\n", wrote, n);

    fclose(file);
    return EXIT_FAILURE;
  }

  fclose(file);
  return EXIT_SUCCESS;
}

static size_t match(size_t n, const uint8_t prog[n], size_t ic) {
  int count;
  if (instr_imm(prog[ic])) {
    count = -1;
    for (--ic; ic < n; --ic) {
      switch (instr_opt(prog[ic])) {
      case BF_ADD:
      case BF_PTR:
      case BF_IOC:
        break;
      case BF_JMP:
        count += instr_imm(prog[ic]) ? 1 : -1;

        if (!count) {
          return ic;
        }
        break;
      }
    }
  } else {
    count = 1;
    for (++ic; ic < n; ++ic) {
      switch (instr_opt(prog[ic])) {
      case BF_ADD:
      case BF_PTR:
      case BF_IOC:
        break;
      case BF_JMP:
        count += instr_imm(prog[ic]) ? 1 : -1;

        if (!count) {
          return ic;
        }
        break;
      }
    }
  }

  return ic;
}

int interpret(size_t n, const uint8_t prog[n], uint32_t flags) {
  uint8_t memory[BFVM_MEM] = {0};
  size_t sp = 0;

  for (size_t ic = 0; ic < n; ++ic) {
    BF_OPT opt = instr_opt(prog[ic]);
    int value = instr_imm(prog[ic]);

    // printf("Line %4lu/%lu Executing 0x%x:%+4d; Stack 0x%016lx:0x%02x\n", ic +
    // 1, n, opt, value, sp, memory[sp]);

    switch (opt) {
    case BF_ADD:
      memory[sp] += value;
      break;
    case BF_PTR:
      sp += value;

      if (BFVM_MEM - 1 < sp) {
        fprintf(stderr, "Error: Memory out of bounds %ld\n", sp);
        return EXIT_FAILURE;
      }
      break;
    case BF_IOC:
      if (value) {
        memory[sp] = getc(stdin);
      } else {
        // printf("0x%02x\n", memory[sp]);
        putc(memory[sp], stdout);
      }
      break;
    case BF_JMP:
      if (flags & BFC_RJMP) {
        if ((memory[sp] == 0) ^ (value < 0)) {
          ic += value;
        }
      } else {
        if ((memory[sp] == 0) ^ (value != 0)) {
          ic = match(n, prog, ic);
        }
      }
      break;
    }
  }

  return EXIT_SUCCESS;
}
