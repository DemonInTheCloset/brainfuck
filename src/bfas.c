#include "bfas.h"

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
