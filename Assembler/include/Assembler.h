#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "Common.h"
#include "My_stack.h"
#include "SPU.h"

#define BYTE_CODE_TOO_LONG  1'001
#define UNKNOWN_ASM_COMMAND 1'002
#define INVALID_REGISTER    1'003
#define INVALID_LABEL       1'004

union Assembler_elem {
    byte_elem_t  command;
    stack_elem_t val;
    byte_elem_t  reg;
    size_t       lab;
    size_t       pos;
};

size_t const BYTE_CODE_MAX_LEN   = 0X1000;

size_t const LABELS_COUNT = 10;

struct Assembler {
    size_t         labels[LABELS_COUNT];
    size_t         code_len;
    char           *code;
    size_t         byte_code_len;
    Assembler_elem *byte_code;

    ON_DEBUG(
    size_t         text_byte_code_len;
    char           *text_byte_code;
    )

    bool           is_valid;
};

errno_t Assembler_Ctor(Assembler *asm_ptr, FILE *code_stream);

void Assembler_Dtor(Assembler *asm_ptr);

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

errno_t compilate(FILE *code_stream, FILE *byte_code_stream
       ON_DEBUG(, FILE *text_byte_code_stream));

#endif
