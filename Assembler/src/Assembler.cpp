#include "Assembler.h"
#include <string.h>

static size_t const BYTE_CODE_MAX_LEN   = 0X1000,
                    ASM_COMMAND_MAX_LEN = 5;

static errno_t HLT_compilate(FILE *const code_stream,
                             size_t *const cur_len,      byte_elem_t *const byte_code
                  ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = HLT_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code   + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX", HLT_COMMAND);
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t PUSH_compilate(FILE *const code_stream,
                              size_t *const cur_len,      byte_elem_t *const byte_code
                   ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = PUSH_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", PUSH_COMMAND);
    )

    assembler_elem_t x = 0;
    CHECK_FUNC(My_fscanf_s, 1, code_stream, assembler_elem_frm, &x);

    *cur_len = get_assembler_aligned(*cur_len + 1);
    if (*cur_len + sizeof(assembler_elem_t) > BYTE_CODE_MAX_LEN) {
        CLEAR_RESOURCES();
        return BYTE_CODE_TOO_LONG;
    }

    *(assembler_elem_t *)(byte_code + *cur_len) = x;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        assembler_elem_frm, x);
             *text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "\n");
    )

    *cur_len += sizeof(assembler_elem_t);
    CLEAR_RESOURCES();
    return 0;
}

static errno_t PUSHR_compilate(FILE *const code_stream,
                               size_t *const cur_len,      byte_elem_t *const byte_code
                    ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = PUSHR_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", PUSHR_COMMAND);
    )

    byte_elem_t reg = 0;
    CHECK_FUNC(My_fscanf_s, 1, code_stream, " r%hhu", &reg);

    ++*cur_len;
    if (*cur_len + 1 > BYTE_CODE_MAX_LEN) { CLEAR_RESOURCES(); return BYTE_CODE_TOO_LONG; }

    if (reg >= SPU_REGS_NUM) { CLEAR_RESOURCES(); return INVALID_REGISTER; }
    byte_code[*cur_len] = reg;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX\n", reg);
             *text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "\n");
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t POP_compilate(FILE *const code_stream,
                             size_t *const cur_len,      byte_elem_t *const byte_code
                  ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = POP_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", POP_COMMAND);
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t POPR_compilate(FILE *const code_stream,
                              size_t *const cur_len,      byte_elem_t *const byte_code
                   ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = POPR_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", POPR_COMMAND);
    )

    byte_elem_t reg = 0;
    CHECK_FUNC(My_fscanf_s, 1, code_stream, " r%hhu", &reg);

    ++*cur_len;
    if (*cur_len + 1 > BYTE_CODE_MAX_LEN) { CLEAR_RESOURCES(); return BYTE_CODE_TOO_LONG; }

    if (reg >= SPU_REGS_NUM) { CLEAR_RESOURCES(); return INVALID_REGISTER; }
    byte_code[*cur_len] = reg;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX\n", reg);
             *text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "\n");
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t ADD_compilate(FILE *const code_stream,
                             size_t *const cur_len,      byte_elem_t *const byte_code
                  ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = ADD_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", ADD_COMMAND);
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t SUB_compilate(FILE *const code_stream,
                             size_t *const cur_len,      byte_elem_t *const byte_code
                  ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = SUB_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", SUB_COMMAND);
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t MLT_compilate(FILE *const code_stream,
                             size_t *const cur_len,      byte_elem_t *const byte_code
                  ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = MLT_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", MLT_COMMAND);
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t DIV_compilate(FILE *const code_stream,
                             size_t *const cur_len,      byte_elem_t *const byte_code
                  ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = DIV_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", DIV_COMMAND);
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t SQRT_compilate(FILE *const code_stream,
                              size_t *const cur_len,      byte_elem_t *const byte_code
                   ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = SQRT_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", SQRT_COMMAND);
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t POW_compilate(FILE *const code_stream,
                             size_t *const cur_len,      byte_elem_t *const byte_code
                  ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = POW_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", POW_COMMAND);
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t IN_compilate(FILE *const code_stream,
                            size_t *const cur_len,      byte_elem_t *const byte_code
                 ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = IN_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", IN_COMMAND);
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t OUT_compilate(FILE *const code_stream,
                             size_t *const cur_len,      byte_elem_t *const byte_code
                  ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = OUT_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", OUT_COMMAND);
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JMP_compilate(FILE *const code_stream,
                             size_t *const cur_len,      byte_elem_t *const byte_code
                  ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = JMP_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", JMP_COMMAND);)

    byte_elem_t pos = 0;
    CHECK_FUNC(My_fscanf_s, 1, code_stream, "%hhu", &pos);

    ++*cur_len;
    if (*cur_len + 1 > BYTE_CODE_MAX_LEN) { CLEAR_RESOURCES(); return BYTE_CODE_TOO_LONG; }

    byte_code[*cur_len] = pos;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhu\n", pos);
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JB_compilate(FILE *const code_stream,
                            size_t *const cur_len,      byte_elem_t *const byte_code
                 ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = JB_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", JB_COMMAND);)

    byte_elem_t pos = 0;
    CHECK_FUNC(My_fscanf_s, 1, code_stream, "%hhu", &pos);

    ++*cur_len;
    if (*cur_len + 1 > BYTE_CODE_MAX_LEN) { CLEAR_RESOURCES(); return BYTE_CODE_TOO_LONG; }

    byte_code[*cur_len] = pos;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhu\n", pos);
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JBE_compilate(FILE *const code_stream,
                             size_t *const cur_len,      byte_elem_t *const byte_code
                  ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = JBE_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", JBE_COMMAND);)

    byte_elem_t pos = 0;
    CHECK_FUNC(My_fscanf_s, 1, code_stream, "%hhu", &pos);

    ++*cur_len;
    if (*cur_len + 1 > BYTE_CODE_MAX_LEN) { CLEAR_RESOURCES(); return BYTE_CODE_TOO_LONG; }

    byte_code[*cur_len] = pos;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhu\n", pos);
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JA_compilate(FILE *const code_stream,
                            size_t *const cur_len,      byte_elem_t *const byte_code
                 ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = JA_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", JA_COMMAND);)

    byte_elem_t pos = 0;
    CHECK_FUNC(My_fscanf_s, 1, code_stream, "%hhu", &pos);

    ++*cur_len;
    if (*cur_len + 1 > BYTE_CODE_MAX_LEN) { CLEAR_RESOURCES(); return BYTE_CODE_TOO_LONG; }

    byte_code[*cur_len] = pos;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhu\n", pos);
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JAE_compilate(FILE *const code_stream,
                             size_t *const cur_len,      byte_elem_t *const byte_code
                  ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = JAE_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", JAE_COMMAND);)

    byte_elem_t pos = 0;
    CHECK_FUNC(My_fscanf_s, 1, code_stream, "%hhu", &pos);

    ++*cur_len;
    if (*cur_len + 1 > BYTE_CODE_MAX_LEN) { CLEAR_RESOURCES(); return BYTE_CODE_TOO_LONG; }

    byte_code[*cur_len] = pos;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhu\n", pos);
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JE_compilate(FILE *const code_stream,
                            size_t *const cur_len,      byte_elem_t *const byte_code
                 ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = JE_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", JE_COMMAND);)

    byte_elem_t pos = 0;
    CHECK_FUNC(My_fscanf_s, 1, code_stream, "%hhu", &pos);

    ++*cur_len;
    if (*cur_len + 1 > BYTE_CODE_MAX_LEN) { CLEAR_RESOURCES(); return BYTE_CODE_TOO_LONG; }

    byte_code[*cur_len] = pos;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhu\n", pos);
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JNE_compilate(FILE *const code_stream,
                             size_t *const cur_len,      byte_elem_t *const byte_code
                  ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code)) {
    assert(code_stream);
             assert(cur_len);      assert(*cur_len      < BYTE_CODE_MAX_LEN); assert(byte_code);
    ON_DEBUG(assert(text_cur_len); assert(*text_cur_len < BYTE_CODE_MAX_LEN); assert(text_byte_code);)

    #undef FINAL_CODE
    #define FINAL_CODE

    byte_code[*cur_len] = JNE_COMMAND;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhX ", JNE_COMMAND);)

    byte_elem_t pos = 0;
    CHECK_FUNC(My_fscanf_s, 1, code_stream, "%hhu", &pos);

    ++*cur_len;
    if (*cur_len + 1 > BYTE_CODE_MAX_LEN) { CLEAR_RESOURCES(); return BYTE_CODE_TOO_LONG; }

    byte_code[*cur_len] = pos;
    ON_DEBUG(*text_cur_len += sprintf_s(text_byte_code    + *text_cur_len,
                                        BYTE_CODE_MAX_LEN - *text_cur_len,
                                        "%hhu\n", pos);
    )

    ++*cur_len;
    CLEAR_RESOURCES();
    return 0;
}

struct Asm_command {
    char const *name;
    errno_t (*compilate_func)(FILE *const code_stream,
                              size_t *const cur_len,      byte_elem_t *const byte_code
                   ON_DEBUG(, size_t *const text_cur_len, char *const text_byte_code));
};

static Asm_command asm_commands[__ASM_COMMAND_COUNT] = {
       Asm_command{"HLT",   &HLT_compilate},
       Asm_command{"PUSH",  &PUSH_compilate},
       Asm_command{"PUSHR", &PUSHR_compilate},
       Asm_command{"POP",   &POP_compilate},
       Asm_command{"POPR",  &POPR_compilate},
       Asm_command{"ADD",   &ADD_compilate},
       Asm_command{"SUB",   &SUB_compilate},
       Asm_command{"MLT",   &MLT_compilate},
       Asm_command{"DIV",   &DIV_compilate},
       Asm_command{"SQRT",  &SQRT_compilate},
       Asm_command{"POW",   &POW_compilate},
       Asm_command{"IN",    &IN_compilate},
       Asm_command{"OUT",   &OUT_compilate},
       Asm_command{"JMP",   &JMP_compilate},
       Asm_command{"JB",    &JB_compilate},
       Asm_command{"JBE",   &JBE_compilate},
       Asm_command{"JA",    &JA_compilate},
       Asm_command{"JAE",   &JAE_compilate},
       Asm_command{"JE",    &JE_compilate},
       Asm_command{"JNE",   &JNE_compilate},
};

errno_t compilate(FILE *const code_stream, FILE *const byte_code_stream
                                ON_DEBUG(, FILE *const text_byte_code_stream)) {
    assert(code_stream); assert(byte_code_stream);
    ON_DEBUG(assert(text_byte_code_stream);)

    #undef FINAL_CODE
    #define FINAL_CODE

    size_t      cur_len    = 0;
    byte_elem_t *byte_code = nullptr;
    CHECK_FUNC(My_calloc, (void **)&byte_code, BYTE_CODE_MAX_LEN, sizeof(*byte_code));
    #undef FINAL_CODE
    #define FINAL_CODE  \
    free(byte_code);

    ON_DEBUG(size_t text_cur_len = 0;
             char *text_byte_code = nullptr;
             CHECK_FUNC(My_calloc, (void **)&text_byte_code, BYTE_CODE_MAX_LEN + 1, sizeof(*text_byte_code));
    #undef FINAL_CODE
    #define FINAL_CODE      \
    free(byte_code);        \
    free(text_byte_code);
    )

    char cur_command[ASM_COMMAND_MAX_LEN + 1] = {};
    while (cur_len      < BYTE_CODE_MAX_LEN ON_DEBUG(and
           text_cur_len < BYTE_CODE_MAX_LEN)) {
        CHECK_FUNC(My_fscanf_s, 1, code_stream, "%s", cur_command, ASM_COMMAND_MAX_LEN + 1); //TODO - make one read

        bool match_any = false;
        for (size_t i = 0; i < __ASM_COMMAND_COUNT; ++i) {
            if (strcmp(cur_command, asm_commands[i].name)) { continue; }

            match_any = true;
            CHECK_FUNC(asm_commands[i].compilate_func, code_stream, &cur_len,      byte_code
                                                         ON_DEBUG(, &text_cur_len, text_byte_code));

            if (i == HLT_COMMAND) {
                CHECK_FUNC(My_fwrite, &cur_len,  sizeof(cur_len),    1,       byte_code_stream);
                CHECK_FUNC(My_fwrite, byte_code, sizeof(*byte_code), cur_len, byte_code_stream);
                ON_DEBUG(fprintf_s(text_byte_code_stream, "%s", text_byte_code));
                CLEAR_RESOURCES();
                return 0;
            }

            break;
        }

        if (!match_any) { CLEAR_RESOURCES(); return UNKNOWN_ASM_COMMAND; }
    }

    CLEAR_RESOURCES();
    return BYTE_CODE_TOO_LONG;
}
