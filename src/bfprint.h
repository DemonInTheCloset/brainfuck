#pragma once
#include "bfdef.h"
#include <stdio.h>

void print_header(BFC_HEADER header);

void pretty_print(size_t n, uint8_t prog[n], const char *indent);
