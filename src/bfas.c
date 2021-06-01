#include "bfas.h"

static const char start_asm[] =
    ".intel_syntax noprefix\n" // Use intel syntax
    ".global _start\n"         // export _start symbol
    ".bss\n"                   // zero initialized memory
    "memory: resb 30000\n"     // allocate memory
    ".text\n"                  // program section
    "_start:\n"                // entry point
    "movabs rax, memory\n";    // load memory address into rax

static const char exit_asm[] = "mov eax, 60\n"  // syscall exit
                               "xor edi, edi\n" // return code 0
                               "syscall\n";     // call

static const char add_asm[] = "add byte ptr [rax], ";       // expects value
static const char ptr_asm[] = "add rax, ";                  // expects value
static const char jmp_asm[] = "movzx esi, byte ptr [rax]\n" // load value
                              "test esi, esi\n";            // test value

static const char jfw_asm[] = "jz ";  // expects label & set label
static const char jbw_asm[] = "jnz "; // expects label & set label

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

char *assembler(size_t n, const uint8_t prog[n]) {
  char buf[512];
  char *output = malloc(sizeof(buf));
  size_t capacity = sizeof(buf) / sizeof(buf[0]);
  size_t length = 0;
  size_t buf_len = 0;

  if (!output) {
    fprintf(stderr, "Error: Failed to allocate %luB\n", capacity);
    return NULL;
  }

  return output;
}
