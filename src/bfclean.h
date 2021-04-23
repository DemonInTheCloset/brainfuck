#pragma once
#include "bfdef.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

BFProgram *make_program(uint64_t length);

BFProgram *cleanup(const char *program, uint64_t prog_length);
