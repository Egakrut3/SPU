#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "Common.h"

size_t const MAX_BYTE_CODE_LEN = 5000;

errno_t make_byte_code(User_error *error_ptr,
                       FILE *code_stream, FILE *byte_code_stream);

#endif
