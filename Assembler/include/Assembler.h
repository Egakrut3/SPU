#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "Common.h"
#include "SPU.h"

#define BYTE_CODE_TOO_LONG  1'001
#define UNKNOWN_ASM_COMMAND 1'002
#define INVALID_REGISTER    1'003

typedef double assembler_elem_t;
char const assembler_elem_frm[] = "%lG";

enum Asm_command_code : byte_elem_t {
    HLT_COMMAND,
    PUSH_COMMAND,
    PUSHR_COMMAND,
    POP_COMMAND,
    POPR_COMMAND,
    ADD_COMMAND,
    SUB_COMMAND,
    MLT_COMMAND,
    DIV_COMMAND,
    SQRT_COMMAND,
    POW_COMMAND,
    IN_COMMAND,
    OUT_COMMAND,
    JMP_COMMAND,
    JB_COMMAND,
    JBE_COMMAND,
    JA_COMMAND,
    JAE_COMMAND,
    JE_COMMAND,
    JNE_COMMAND,
    __ASM_COMMAND_COUNT,
};

size_t get_assembler_aligned(size_t x);

errno_t compilate(FILE *code_stream, FILE *byte_code_stream
       ON_DEBUG(, FILE *text_byte_code_stream));

#endif
