#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "Common.h"

typedef double assembler_elem_t;
char const assembler_elem_frm[] = "%lG";

size_t get_assembler_aligned(size_t x);

errno_t make_byte_code(User_error *error_ptr,
                       FILE *code_stream, FILE *byte_code_stream
                       ON_DEBUG(, FILE *text_byte_code_stream));

#endif
