#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "Common.h"
#include "My_stack.h"

#define UNKNOWN_ASM_COMMAND 2'000
#define INVALID_REGISTER    2'001
#define INVALID_LABEL       2'002
#define BYTE_CODE_TOO_LONG  2'003

union Assembler_elem {
    byte_elem_t  command;
    stack_elem_t val;
    byte_elem_t  reg;
    size_t       lab;
    size_t       pos;
};

size_t const BYTE_CODE_MAX_LEN   = 0X1000;

size_t const LABELS_NUM = 10;
size_t const REGS_NUM = 16;
size_t const DEFAULT_POSITION = ~0u;

struct Assembler {
    size_t         labels[LABELS_NUM];
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

#undef HANDLE_COMMAND
#define HANDLE_COMMAND(name)    \
name ## _COMMAND,

enum Asm_command_code : byte_elem_t {
    //TODO -
    #include "Command_list.h"
};

errno_t compilate(FILE *code_stream, FILE *byte_code_stream
       ON_DEBUG(, FILE *text_byte_code_stream));

#endif
