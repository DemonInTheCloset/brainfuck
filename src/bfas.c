#include "bfas.h"

static const char start_asm[] =
    ".intel_syntax noprefix\n" // Use intel syntax
    ".global _start\n"         // export _start symbol
    ".bss\n"                   // zero initialized memory
    ".lcomm memory, 30000\n"   // allocate memory
    ".text\n"                  // program section
    "_start:\n"                // entry point
    "movabs rax, memory\n";    // load memory address into rax

static const char exit_asm[] = "mov eax, 60\n"  // syscall exit
                               "xor edi, edi\n" // return code 0
                               "syscall\n";     // call

static const char add_asm[] = "add byte ptr [rax], 0x";     // expects value
static const char ptr_asm[] = "add rax, ";                  // expects value
static const char jmp_asm[] = "movzx esi, byte ptr [rax]\n" // load value
                              "test esi, esi\n";            // test value

static const char label[] = ".L0x";       // jump label
static const char jfw_asm[] = "jz .L0x";  // expects label & set label
static const char jbw_asm[] = "jnz .L0x"; // expects label & set label

static const char ioc_asm[] = "push rax\n"      // save value of rax
                              "mov edx, 1\n"    // number of bytes to read/write
                              "mov rsi, rax\n"; // address
static const char put_asm[] = "mov edi, 1\n"    // stdout
                              "mov eax, 1\n"    // syscall write
                              "syscall\n"       // call
                              "pop rax\n";      // restore value from rax
static const char get_asm[] = "mov edi, 0\n"    // stdin
                              "mov eax, 0\n"    // syscall write
                              "syscall\n"       // call
                              "pop rax\n";      // restore value from rax

static size_t generate_asm(size_t n, char buffer[n], uint8_t instr, size_t ic) {
  size_t written = 0;
  int value = instr_imm(instr);

  switch (instr_opt(instr)) {
  case BF_ADD: {
    written = strlen(add_asm) + 4;

    if (written > n)
      return 0;

    sprintf(buffer, "%s%02x\n", add_asm, (uint8_t)value);
    break;
  }
  case BF_PTR: {
    written = strlen(ptr_asm) + 6;

    if (written > n)
      return 0;

    sprintf(buffer, "%s%+4d\n", ptr_asm, value);
    break;
  }
  case BF_JMP: {
    const char *jmp = (value < 0) ? jbw_asm : jfw_asm;
    written = strlen(jmp_asm) + strlen(jmp) + strlen(label) + 36;

    if (written > n)
      return 0;

    sprintf(buffer,
            "%s"     // value test
            "%s"     // jump
            "%016lx" // label to
            "\n"     // next command
            "%s"     // label from
            "%016lx"
            ":\n",
            jmp_asm, jmp, ic + value, label, ic);
    break;
  }
  case BF_IOC: {
    const char *ioc = (value == 0) ? put_asm : get_asm;
    written = strlen(ioc_asm) + strlen(ioc) + 1;

    if (written > n)
      return 0;

    sprintf(buffer, "%s%s", ioc_asm, ioc);
    break;
  }
  }

  return written - 1;
}

char *assembler(size_t n, const uint8_t prog[n]) {
  char buf[512];
  size_t out_len = 0;
  size_t buf_len = 0;
  size_t capacity = strlen(start_asm) + strlen(exit_asm) + sizeof(buf);
  char *output = NULL;

  if (!(output = realloc(output, capacity))) {
    fprintf(stderr, "Error: Failed to allocate %luB\n", capacity);
    return NULL;
  }

  memcpy(output + out_len, start_asm, strlen(start_asm));
  out_len += strlen(start_asm);

  for (size_t ic = 0; ic < n; ++ic) {
    buf_len = generate_asm(sizeof(buf), buf, prog[ic], ic);

    if (buf_len == 0) {
      fprintf(stderr, "Error: Failed to generate assembly\n");
      return NULL;
    }

    if (out_len + buf_len > capacity) {
      capacity += capacity / 2;

      if (!(output = realloc(output, capacity))) {
        fprintf(stderr, "Error: Failed to allocate %luB\n", capacity);
        return NULL;
      }
    }

    memcpy(output + out_len, buf, buf_len);
    out_len += buf_len;
  }

  if (out_len + strlen(exit_asm) + 1 > capacity) {
    capacity = out_len + strlen(exit_asm) + 1;

    if (!(output = realloc(output, capacity))) {
      fprintf(stderr, "Error: Failed to allocate %luB\n", capacity);
      return NULL;
    }
  }

  memcpy(output + out_len, exit_asm, strlen(exit_asm) + 1);

  return output;
}
