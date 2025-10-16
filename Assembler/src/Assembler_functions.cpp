#include "Assembler.h"
#include <string.h>

static size_t const ASM_COMMAND_MAX_LEN = 5;

#undef FINAL_CODE
#define FINAL_CODE

static errno_t HLT_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = HLT_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX\n", HLT_COMMAND);
    )

    return 0;
}

static errno_t PUSH_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = PUSH_COMMAND; //TODO - possible macros
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX ", PUSH_COMMAND);
    )

    stack_elem_t val = {};
    size_t extra_len = 0;
    CHECK_FUNC(My_sscanf_s, 1, asm_ptr->code + *cur_char_ptr, STACK_ELEM_FRM "%n", &val, &extra_len);
    *cur_char_ptr += extra_len;
    if (asm_ptr->byte_code_len      >= BYTE_CODE_MAX_LEN ON_DEBUG(or
        asm_ptr->text_byte_code_len >= BYTE_CODE_MAX_LEN)) {
        return BYTE_CODE_TOO_LONG;
    }
    asm_ptr->byte_code[asm_ptr->byte_code_len++].val = val;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                             BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                             STACK_ELEM_FRM "\n", val);
    )

    return 0;
}

static errno_t PUSHR_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = PUSHR_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX ", PUSHR_COMMAND);
    )

    byte_elem_t reg = {};
    size_t extra_len = 0;
    CHECK_FUNC(My_sscanf_s, 1, asm_ptr->code + *cur_char_ptr, " r%hhu%n", &reg, &extra_len);
    *cur_char_ptr += extra_len;
    if (reg >= REGS_NUM) {
        return INVALID_REGISTER;
    }

    if (asm_ptr->byte_code_len      >= BYTE_CODE_MAX_LEN ON_DEBUG(or
        asm_ptr->text_byte_code_len >= BYTE_CODE_MAX_LEN)) {
        return BYTE_CODE_TOO_LONG;
    }
    asm_ptr->byte_code[asm_ptr->byte_code_len++].reg = reg;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                             BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                             "%hhX\n", reg);
    )

    return 0;
}

static errno_t POP_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = POP_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX\n", POP_COMMAND);
    )

    return 0;
}

static errno_t POPR_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = POPR_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX ", POPR_COMMAND);
    )

    byte_elem_t reg = {};
    size_t extra_len = 0;
    CHECK_FUNC(My_sscanf_s, 1, asm_ptr->code + *cur_char_ptr, " r%hhu%n", &reg, &extra_len);
    *cur_char_ptr += extra_len;
    if (reg >= REGS_NUM) {
        return INVALID_REGISTER;
    }

    if (asm_ptr->byte_code_len      >= BYTE_CODE_MAX_LEN ON_DEBUG(or
        asm_ptr->text_byte_code_len >= BYTE_CODE_MAX_LEN)) {
        return BYTE_CODE_TOO_LONG;
    }
    asm_ptr->byte_code[asm_ptr->byte_code_len++].reg = reg;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                             BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                             "%hhX\n", reg);
    )

    return 0;
}

static errno_t ADD_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = ADD_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX\n", ADD_COMMAND);
    )

    return 0;
}

static errno_t SUB_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = SUB_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX\n", SUB_COMMAND);
    )

    return 0;
}

static errno_t MLT_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = MLT_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX\n", MLT_COMMAND);
    )

    return 0;
}

static errno_t DIV_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = DIV_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX\n", DIV_COMMAND);
    )

    return 0;
}

static errno_t SQRT_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = SQRT_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX\n", SQRT_COMMAND);
    )

    return 0;
}

static errno_t POW_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = POW_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX\n", POW_COMMAND);
    )

    return 0;
}

static errno_t IN_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = IN_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX\n", IN_COMMAND);
    )

    return 0;
}

static errno_t OUT_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = OUT_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX\n", OUT_COMMAND);
    )

    return 0;
}

static errno_t JMP_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = JMP_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX ", JMP_COMMAND);
    )

    size_t lab = {};
    size_t extra_len = 0;
    CHECK_FUNC(My_sscanf_s, 1, asm_ptr->code + *cur_char_ptr, "%zu%n", &lab, &extra_len);
    *cur_char_ptr += extra_len;
    if (lab >= LABELS_COUNT) {
        return INVALID_LABEL;
    }

    if (asm_ptr->byte_code_len      >= BYTE_CODE_MAX_LEN ON_DEBUG(or
        asm_ptr->text_byte_code_len >= BYTE_CODE_MAX_LEN)) {
        return BYTE_CODE_TOO_LONG;
    }

    if (asm_ptr->labels[lab] == DEFAULT_POSITION) {
        return INVALID_LABEL;
    }
    asm_ptr->byte_code[asm_ptr->byte_code_len++].pos = asm_ptr->labels[lab];
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                             BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                             "%zu\n", asm_ptr->labels[lab]);
    )

    return 0;
}

static errno_t JB_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = JB_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX ", JB_COMMAND);
    )

    size_t lab = {};
    size_t extra_len = 0;
    CHECK_FUNC(My_sscanf_s, 1, asm_ptr->code + *cur_char_ptr, "%zu%n", &lab, &extra_len);
    *cur_char_ptr += extra_len;
    if (lab >= LABELS_COUNT) {
        return INVALID_LABEL;
    }

    if (asm_ptr->byte_code_len      >= BYTE_CODE_MAX_LEN ON_DEBUG(or
        asm_ptr->text_byte_code_len >= BYTE_CODE_MAX_LEN)) {
        return BYTE_CODE_TOO_LONG;
    }

    if (asm_ptr->labels[lab] == DEFAULT_POSITION) {
        return INVALID_LABEL;
    }
    asm_ptr->byte_code[asm_ptr->byte_code_len++].pos = asm_ptr->labels[lab];
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                             BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                             "%zu\n", asm_ptr->labels[lab]);
    )

    return 0;
}

static errno_t JBE_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = JBE_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX ", JBE_COMMAND);
    )

    size_t lab = {};
    size_t extra_len = 0;
    CHECK_FUNC(My_sscanf_s, 1, asm_ptr->code + *cur_char_ptr, "%zu%n", &lab, &extra_len);
    *cur_char_ptr += extra_len;
    if (lab >= LABELS_COUNT) {
        return INVALID_LABEL;
    }

    if (asm_ptr->byte_code_len      >= BYTE_CODE_MAX_LEN ON_DEBUG(or
        asm_ptr->text_byte_code_len >= BYTE_CODE_MAX_LEN)) {
        return BYTE_CODE_TOO_LONG;
    }

    if (asm_ptr->labels[lab] == DEFAULT_POSITION) {
        return INVALID_LABEL;
    }
    asm_ptr->byte_code[asm_ptr->byte_code_len++].pos = asm_ptr->labels[lab];
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                             BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                             "%zu\n", asm_ptr->labels[lab]);
    )

    return 0;
}

static errno_t JA_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = JA_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX ", JA_COMMAND);
    )

    size_t lab = {};
    size_t extra_len = 0;
    CHECK_FUNC(My_sscanf_s, 1, asm_ptr->code + *cur_char_ptr, "%zu%n", &lab, &extra_len);
    *cur_char_ptr += extra_len;
    if (lab >= LABELS_COUNT) {
        return INVALID_LABEL;
    }

    if (asm_ptr->byte_code_len      >= BYTE_CODE_MAX_LEN ON_DEBUG(or
        asm_ptr->text_byte_code_len >= BYTE_CODE_MAX_LEN)) {
        return BYTE_CODE_TOO_LONG;
    }

    if (asm_ptr->labels[lab] == DEFAULT_POSITION) {
        return INVALID_LABEL;
    }
    asm_ptr->byte_code[asm_ptr->byte_code_len++].pos = asm_ptr->labels[lab];
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                             BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                             "%zu\n", asm_ptr->labels[lab]);
    )

    return 0;
}

static errno_t JAE_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = JAE_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX ", JAE_COMMAND);
    )

    size_t lab = {};
    size_t extra_len = 0;
    CHECK_FUNC(My_sscanf_s, 1, asm_ptr->code + *cur_char_ptr, "%zu%n", &lab, &extra_len);
    *cur_char_ptr += extra_len;
    if (lab >= LABELS_COUNT) {
        return INVALID_LABEL;
    }

    if (asm_ptr->byte_code_len      >= BYTE_CODE_MAX_LEN ON_DEBUG(or
        asm_ptr->text_byte_code_len >= BYTE_CODE_MAX_LEN)) {
        return BYTE_CODE_TOO_LONG;
    }

    if (asm_ptr->labels[lab] == DEFAULT_POSITION) {
        return INVALID_LABEL;
    }
    asm_ptr->byte_code[asm_ptr->byte_code_len++].pos = asm_ptr->labels[lab];
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                             BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                             "%zu\n", asm_ptr->labels[lab]);
    )

    return 0;
}

static errno_t JE_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = JE_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX ", JE_COMMAND);
    )

    size_t lab = {};
    size_t extra_len = 0;
    CHECK_FUNC(My_sscanf_s, 1, asm_ptr->code + *cur_char_ptr, "%zu%n", &lab, &extra_len);
    *cur_char_ptr += extra_len;
    if (lab >= LABELS_COUNT) {
        return INVALID_LABEL;
    }

    if (asm_ptr->byte_code_len      >= BYTE_CODE_MAX_LEN ON_DEBUG(or
        asm_ptr->text_byte_code_len >= BYTE_CODE_MAX_LEN)) {
        return BYTE_CODE_TOO_LONG;
    }

    if (asm_ptr->labels[lab] == DEFAULT_POSITION) {
        return INVALID_LABEL;
    }
    asm_ptr->byte_code[asm_ptr->byte_code_len++].pos = asm_ptr->labels[lab];
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                             BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                             "%zu\n", asm_ptr->labels[lab]);
    )

    return 0;
}

static errno_t JNE_compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = JNE_COMMAND;
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                            "%hhX ", JNE_COMMAND);
    )

    size_t lab = {};
    size_t extra_len = 0;
    CHECK_FUNC(My_sscanf_s, 1, asm_ptr->code + *cur_char_ptr, "%zu%n", &lab, &extra_len);
    *cur_char_ptr += extra_len;
    if (lab >= LABELS_COUNT) {
        return INVALID_LABEL;
    }

    if (asm_ptr->byte_code_len      >= BYTE_CODE_MAX_LEN ON_DEBUG(or
        asm_ptr->text_byte_code_len >= BYTE_CODE_MAX_LEN)) {
        return BYTE_CODE_TOO_LONG;
    }

    if (asm_ptr->labels[lab] == DEFAULT_POSITION) {
        return INVALID_LABEL;
    }
    asm_ptr->byte_code[asm_ptr->byte_code_len++].pos = asm_ptr->labels[lab];
    ON_DEBUG(
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,
                                             BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,
                                             "%zu\n", asm_ptr->labels[lab]);
    )

    return 0;
}

static errno_t add_label(Assembler *const asm_ptr, size_t *const cur_char_ptr,
                         char const *const name, size_t const command_count) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    size_t lab = {};
    CHECK_FUNC(My_sscanf_s, 1, name, "%zu", &lab);
    if (lab >= LABELS_COUNT or asm_ptr->labels[lab] != DEFAULT_POSITION) {
        return INVALID_LABEL;
    }
    asm_ptr->labels[lab] = command_count;

    return 0;
}

errno_t compilate(FILE *const code_stream, FILE *const byte_code_stream
                                ON_DEBUG(, FILE *const text_byte_code_stream)) {
    assert(code_stream); assert(byte_code_stream);
    ON_DEBUG(assert(text_byte_code_stream);)

    #undef FINAL_CODE
    #define FINAL_CODE

    Assembler cur_asm = {};
    CHECK_FUNC(Assembler_Ctor, &cur_asm, code_stream);
    #undef FINAL_CODE
    #define FINAL_CODE              \
        Assembler_Dtor(&cur_asm);

    char cur_command[ASM_COMMAND_MAX_LEN + 1] = {};
    size_t cur_char      = 0,
           command_count = 0;
    while (cur_char < cur_asm.code_len) {
        size_t extra_len = 0;
        CHECK_FUNC(My_sscanf_s, 1, cur_asm.code + cur_char, "%s %n",
                                   cur_command, ASM_COMMAND_MAX_LEN + 1, &extra_len);
        cur_char += extra_len;

        if (cur_command[0] == ':') {
            CHECK_FUNC(add_label, &cur_asm, &cur_char, cur_command + 1, command_count);
            continue;
        }

        ++command_count;
    }

    cur_char = 0;
    while (cur_char                   < cur_asm.code_len  and
           cur_asm.byte_code_len      < BYTE_CODE_MAX_LEN ON_DEBUG(and
           cur_asm.text_byte_code_len < BYTE_CODE_MAX_LEN)) {
        size_t extra_len = 0;
        CHECK_FUNC(My_sscanf_s, 1, cur_asm.code + cur_char, "%s %n",
                                   cur_command, ASM_COMMAND_MAX_LEN + 1, &extra_len);
        cur_char += extra_len;

        #define CHECK_COMMAND(name)                                 \
        if (!strcmp(cur_command, #name)) {                          \
            CHECK_FUNC(name ## _compilate, &cur_asm, &cur_char);    \
            continue;                                               \
        }

        CHECK_COMMAND(HLT);

        CHECK_COMMAND(PUSH);
        CHECK_COMMAND(PUSHR);
        CHECK_COMMAND(POP);
        CHECK_COMMAND(POPR);

        CHECK_COMMAND(ADD);
        CHECK_COMMAND(SUB);
        CHECK_COMMAND(MLT);
        CHECK_COMMAND(DIV);

        CHECK_COMMAND(SQRT);
        CHECK_COMMAND(POW);

        CHECK_COMMAND(IN);
        CHECK_COMMAND(OUT);

        CHECK_COMMAND(JMP);
        CHECK_COMMAND(JB);
        CHECK_COMMAND(JBE);
        CHECK_COMMAND(JA);
        CHECK_COMMAND(JAE);
        CHECK_COMMAND(JE);
        CHECK_COMMAND(JNE);

        if (cur_command[0] == ':') {
            continue;
        }

        CLEAR_RESOURCES();
        return UNKNOWN_ASM_COMMAND;
    }

    if (cur_char < cur_asm.code_len) {
        CLEAR_RESOURCES();
        return BYTE_CODE_TOO_LONG;
    }

    CHECK_FUNC(My_fwrite, &cur_asm.byte_code_len, sizeof(cur_asm.byte_code_len), 1,
                          byte_code_stream);
    CHECK_FUNC(My_fwrite, cur_asm.byte_code, sizeof(*cur_asm.byte_code), cur_asm.byte_code_len,
                          byte_code_stream);
    ON_DEBUG(fprintf_s(text_byte_code_stream, "%s", cur_asm.text_byte_code);)

    CLEAR_RESOURCES();
    return 0;
}
