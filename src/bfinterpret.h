#pragma once
#include "bfdef.h"
#include <stdio.h>
#include <stdlib.h>

BFC_HEADER read_bytecode(const char *path, uint8_t **prog);

int write_bytecode(size_t n, const uint8_t prog[n], uint32_t flags,
                   const char *path);

int interpret(size_t n, const uint8_t prog[n], uint32_t flags);
