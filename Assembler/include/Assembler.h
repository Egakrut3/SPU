#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "Common.h"
#include "SPU.h"

#define BYTE_CODE_TOO_LONG  1'001
#define UNKNOWN_ASM_COMMAND 1'002
#define INVALID_REGISTER    1'003

typedef double assembler_elem_t;
char const assembler_elem_frm[] = "%lG";

errno_t compilate(FILE *code_stream, FILE *byte_code_stream
       ON_DEBUG(, FILE *text_byte_code_stream));

#endif
