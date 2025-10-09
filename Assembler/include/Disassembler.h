#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include "Common.h"
#include "SPU.h"

#define CODE_TOO_LONG 1'008

errno_t disassembly_byte_code(FILE *byte_code_stream, FILE *code_stream);

#endif
