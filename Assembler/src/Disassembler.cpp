#include "Disassembler.h"

static size_t const CODE_MAX_LEN = 0X1000;

static errno_t HLT_disassembly(size_t *const IC_ptr,
                               size_t const byte_code_len, byte_elem_t const *const byte_code,
                               size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == HLT_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr, "HLT");

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t PUSH_disassembly(size_t *const IC_ptr,
                                size_t const byte_code_len, byte_elem_t const *const byte_code,
                                size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == PUSH_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    *IC_ptr = get_assembler_aligned(*IC_ptr + 1);
    if (*IC_ptr + sizeof(assembler_elem_t) > byte_code_len) {
        CLEAR_RESOURCES();
        return INCOMPLETE_ARGUMENT;
    }

    assembler_elem_t const x = *(assembler_elem_t const *)(byte_code + *IC_ptr);
    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr, "PUSH ");
    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr, assembler_elem_frm, x);
    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr, "\n");

    *IC_ptr += sizeof(assembler_elem_t);
    CLEAR_RESOURCES();
    return 0;
}

static errno_t PUSHR_disassembly(size_t *const IC_ptr,
                                 size_t const byte_code_len, byte_elem_t const *const byte_code,
                                 size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == PUSHR_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    ++*IC_ptr;
    if (*IC_ptr + 1 > byte_code_len) { CLEAR_RESOURCES(); return INCOMPLETE_ARGUMENT; }

    byte_elem_t const reg = byte_code[*IC_ptr];
    if (reg >= SPU_REGS_NUM) { CLEAR_RESOURCES(); return INVALID_REGISTER; }
    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr, "PUSHR r%hhu\n", reg);

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t POP_disassembly(size_t *const IC_ptr,
                               size_t const byte_code_len, byte_elem_t const *const byte_code,
                               size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == POP_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr, "POP\n");

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t POPR_disassembly(size_t *const IC_ptr,
                                size_t const byte_code_len, byte_elem_t const *const byte_code,
                                size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == POPR_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    ++*IC_ptr;
    if (*IC_ptr + 1 > byte_code_len) { CLEAR_RESOURCES(); return INCOMPLETE_ARGUMENT; }

    byte_elem_t const reg = byte_code[*IC_ptr];
    if (reg >= SPU_REGS_NUM) { CLEAR_RESOURCES(); return INVALID_REGISTER; }
    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr, "POPR r%hhu\n", reg);

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t ADD_disassembly(size_t *const IC_ptr,
                               size_t const byte_code_len, byte_elem_t const *const byte_code,
                               size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == ADD_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr, "ADD\n");

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t SUB_disassembly(size_t *const IC_ptr,
                               size_t const byte_code_len, byte_elem_t const *const byte_code,
                               size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == SUB_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr, "SUB\n");

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t MLT_disassembly(size_t *const IC_ptr,
                               size_t const byte_code_len, byte_elem_t const *const byte_code,
                               size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == MLT_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr, "MLT\n");

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t DIV_disassembly(size_t *const IC_ptr,
                               size_t const byte_code_len, byte_elem_t const *const byte_code,
                               size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == DIV_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr, "DIV\n");

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t SQRT_disassembly(size_t *const IC_ptr,
                                size_t const byte_code_len, byte_elem_t const *const byte_code,
                                size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == SQRT_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr, "SQRT\n");

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t POW_disassembly(size_t *const IC_ptr,
                               size_t const byte_code_len, byte_elem_t const *const byte_code,
                               size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == POW_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr, "POW\n");

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t IN_disassembly(size_t *const IC_ptr,
                              size_t const byte_code_len, byte_elem_t const *const byte_code,
                              size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == IN_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr, "IN\n");

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t OUT_disassembly(size_t *const IC_ptr,
                               size_t const byte_code_len, byte_elem_t const *const byte_code,
                               size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == OUT_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr, "OUT\n");

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JMP_disassembly(size_t *const IC_ptr,
                               size_t const byte_code_len, byte_elem_t const *const byte_code,
                               size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == JMP_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    ++*IC_ptr;
    if (*IC_ptr + 1 > byte_code_len) { CLEAR_RESOURCES(); return INCOMPLETE_ARGUMENT; }

    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr,
                              "JMP %hhu\n", byte_code[*IC_ptr]);

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JB_disassembly(size_t *const IC_ptr,
                              size_t const byte_code_len, byte_elem_t const *const byte_code,
                              size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == JB_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    ++*IC_ptr;
    if (*IC_ptr + 1 > byte_code_len) { CLEAR_RESOURCES(); return INCOMPLETE_ARGUMENT; }

    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr,
                              "JB %hhu\n", byte_code[*IC_ptr]);

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JBE_disassembly(size_t *const IC_ptr,
                               size_t const byte_code_len, byte_elem_t const *const byte_code,
                               size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == JBE_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    ++*IC_ptr;
    if (*IC_ptr + 1 > byte_code_len) { CLEAR_RESOURCES(); return INCOMPLETE_ARGUMENT; }

    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr,
                              "JBE %hhu\n", byte_code[*IC_ptr]);

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JA_disassembly(size_t *const IC_ptr,
                              size_t const byte_code_len, byte_elem_t const *const byte_code,
                              size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == JA_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    ++*IC_ptr;
    if (*IC_ptr + 1 > byte_code_len) { CLEAR_RESOURCES(); return INCOMPLETE_ARGUMENT; }

    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr,
                              "JA %hhu\n", byte_code[*IC_ptr]);

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JAE_disassembly(size_t *const IC_ptr,
                               size_t const byte_code_len, byte_elem_t const *const byte_code,
                               size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == JAE_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    ++*IC_ptr;
    if (*IC_ptr + 1 > byte_code_len) { CLEAR_RESOURCES(); return INCOMPLETE_ARGUMENT; }

    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr,
                              "JAE %hhu\n", byte_code[*IC_ptr]);

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JE_disassembly(size_t *const IC_ptr,
                              size_t const byte_code_len, byte_elem_t const *const byte_code,
                              size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == JE_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    ++*IC_ptr;
    if (*IC_ptr + 1 > byte_code_len) { CLEAR_RESOURCES(); return INCOMPLETE_ARGUMENT; }

    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr,
                              "JE %hhu\n", byte_code[*IC_ptr]);

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JNE_disassembly(size_t *const IC_ptr,
                               size_t const byte_code_len, byte_elem_t const *const byte_code,
                               size_t *const cur_len_ptr,  char *const code) {
    assert(IC_ptr); assert(byte_code);
    assert(*IC_ptr < byte_code_len); assert(byte_code[*IC_ptr] == JNE_COMMAND);
    assert(cur_len_ptr); assert(*cur_len_ptr < CODE_MAX_LEN); assert(code);

#undef FINAL_CODE
#define FINAL_CODE

    ++*IC_ptr;
    if (*IC_ptr + 1 > byte_code_len) { CLEAR_RESOURCES(); return INCOMPLETE_ARGUMENT; }

    *cur_len_ptr += sprintf_s(code + *cur_len_ptr, CODE_MAX_LEN - *cur_len_ptr,
                              "JNE %hhu\n", byte_code[*IC_ptr]);

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t (*const disassembly_functions[__ASM_COMMAND_COUNT])(size_t *,
                                                                   size_t, byte_elem_t const *,
                                                                   size_t *, char *) = {
    &HLT_disassembly,
    &PUSH_disassembly,
    &PUSHR_disassembly,
    &POP_disassembly,
    &POPR_disassembly,
    &ADD_disassembly,
    &SUB_disassembly,
    &MLT_disassembly,
    &DIV_disassembly,
    &SQRT_disassembly,
    &POW_disassembly,
    &IN_disassembly,
    &OUT_disassembly,
    &JMP_disassembly,
    &JB_disassembly,
    &JBE_disassembly,
    &JA_disassembly,
    &JAE_disassembly,
    &JE_disassembly,
    &JNE_disassembly,
};

errno_t disassembly_byte_code(FILE *byte_code_stream, FILE *code_stream) {
    assert(byte_code_stream); assert(code_stream);

    size_t byte_code_len = 0;
    byte_elem_t *byte_code = nullptr;
    size_t cur_len = 0;
    char *code = nullptr;
#undef FINAL_CODE
#define FINAL_CODE      \
    free(byte_code);    \
    free(code);

    if (fread(&byte_code_len, sizeof(size_t), 1, byte_code_stream) != 1) {
        PRINT_LINE();
        fprintf_s(stderr, "fread failed\n");
        CLEAR_RESOURCES();
        return ferror(byte_code_stream);
    }
    CHECK_FUNC(My_calloc, (void **)&byte_code, byte_code_len, sizeof(*byte_code));
    if (fread(byte_code, sizeof(char), byte_code_len, byte_code_stream) != byte_code_len) {
        PRINT_LINE();
        fprintf_s(stderr, "fread failed\n");
        CLEAR_RESOURCES();
        return ferror(byte_code_stream);
    }

    CHECK_FUNC(My_calloc, (void **)&code, CODE_MAX_LEN + 1, sizeof(*code));

    size_t IC = 0;
    fprintf_s(stderr, "byte_code_len = %zu\n", byte_code_len);
    while (IC < byte_code_len and cur_len < CODE_MAX_LEN) {
        byte_elem_t const cur_command = byte_code[IC];
        if (cur_command >= __ASM_COMMAND_COUNT) { CLEAR_RESOURCES(); return UNKNOWN_COMMAND; }

        CHECK_FUNC(disassembly_functions[cur_command], &IC, byte_code_len, byte_code, &cur_len, code);

        if (cur_command == HLT_COMMAND) {
            CHECK_FUNC(My_fwrite, code, sizeof(*code), cur_len, code_stream);
            return 0;
        }
    }

    if (IC < byte_code_len) { CLEAR_RESOURCES(); return CODE_TOO_LONG; }
    else                    { CLEAR_RESOURCES(); return NO_HLT_COMMAND_REACHED; }
}
