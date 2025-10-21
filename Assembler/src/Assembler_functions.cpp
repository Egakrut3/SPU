#include "Assembler.h"
#include <string.h>

static size_t const ASM_COMMAND_MAX_LEN = 5;

enum Asm_arg_type {
    REG_TYPE,
    LAB_TYPE,
    POS_TYPE,
    VAL_TYPE,
    COMMAND_TYPE,
};

struct Asm_arg {
    Assembler_elem elem;
    Asm_arg_type   type;
};

#undef FINAL_CODE
#define FINAL_CODE

static errno_t get_arg(Assembler const *const asm_ptr, size_t *const cur_char_ptr,
                       Asm_arg *const arg_ptr) {
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);
    assert(arg_ptr);
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);)

    size_t extra_len = {};
    //TODO - possible unsafe conversations

    if (sscanf_s(asm_ptr->code + *cur_char_ptr, " r%hhu %zn", &arg_ptr->elem.reg, &extra_len)) {
        if (arg_ptr->elem.reg >= REGS_NUM) {
            return INVALID_REGISTER;
        }

        *cur_char_ptr += extra_len;

        arg_ptr->type = REG_TYPE;
        return 0;
    }

    if (sscanf_s(asm_ptr->code + *cur_char_ptr, " :%hhu %zn", &arg_ptr->elem.lab, &extra_len)) {
        if (arg_ptr->elem.lab >= LABELS_NUM) {
            return INVALID_LABEL;
        }

        *cur_char_ptr += extra_len;

        arg_ptr->type = LAB_TYPE;
        return 0;
    }

    size_t cur_lab = {};
    if (sscanf_s(asm_ptr->code + *cur_char_ptr, " ::%hhu %zn", &cur_lab, &extra_len)) {
        if (cur_lab >= LABELS_NUM) {
            return INVALID_LABEL;
        }

        arg_ptr->elem.pos = asm_ptr->labels[cur_lab];
        *cur_char_ptr += extra_len;

        arg_ptr->type = POS_TYPE;
        return 0;
    }

    if (sscanf_s(asm_ptr->code + *cur_char_ptr, STACK_ELEM_FRM " %zn", &arg_ptr->elem.val, &extra_len)) {
        *cur_char_ptr += extra_len;

        arg_ptr->type = VAL_TYPE;
        return 0;
    }

    char cur_command[ASM_COMMAND_MAX_LEN + 1] = {};
    if (sscanf_s(asm_ptr->code + *cur_char_ptr, "%s %zn", cur_command, ASM_COMMAND_MAX_LEN + 1,
                                                         &extra_len)) {
        *cur_char_ptr += extra_len;

        #define CHECK_COMMAND(name)                     \
        if (!strcmp(cur_command, #name)) {              \
            arg_ptr->elem.command = name ## _COMMAND;   \
                                                        \
            arg_ptr->type = COMMAND_TYPE;               \
            return 0;                                   \
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

        CHECK_COMMAND(CALL);
        CHECK_COMMAND(RET);

        CHECK_COMMAND(PUSHM);
        CHECK_COMMAND(POPM);

        CHECK_COMMAND(DRAW);

        #undef CHECK_COMMAND

        return UNKNOWN_ASM_COMMAND;
    }

    return UNKNOWN_ASM_TYPE;
}

#define MAKE_SIMPLE_COMPILATE(name)                                                                     \
static errno_t name ## _compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {               \
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);            \
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);       \
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);) \
                                                                                                        \
    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = name ## _COMMAND;                            \
    ON_DEBUG(                                                                                           \
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,     \
                                             BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,           \
                                             "%hhX\n", name ## _COMMAND);                               \
    )                                                                                                   \
                                                                                                        \
    return 0;                                                                                           \
}

#define MAKE_VAL_COMPILATE(name)                                                                        \
static errno_t name ## _compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {               \
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);            \
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);       \
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);) \
                                                                                                        \
    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = name ## _COMMAND;                            \
    ON_DEBUG(                                                                                           \
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,     \
                                             BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,           \
                                             "%hhX ", name ## _COMMAND);                                \
    )                                                                                                   \
                                                                                                        \
    Asm_arg arg = {};                                                                                   \
    CHECK_FUNC(get_arg, asm_ptr, cur_char_ptr, &arg);                                                   \
                                                                                                        \
    if (asm_ptr->byte_code_len      >= BYTE_CODE_MAX_LEN ON_DEBUG(or                                    \
        asm_ptr->text_byte_code_len >= BYTE_CODE_MAX_LEN)) {                                            \
        return BYTE_CODE_TOO_LONG;                                                                      \
    }                                                                                                   \
    asm_ptr->byte_code[asm_ptr->byte_code_len++].val = arg.elem.val;                                    \
    ON_DEBUG(                                                                                           \
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,     \
                                             BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,           \
                                             STACK_ELEM_FRM "\n", arg.elem.val);                        \
    )                                                                                                   \
                                                                                                        \
    return 0;                                                                                           \
}

#define MAKE_REG_COMPILATE(name)                                                                        \
static errno_t name ## _compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {               \
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);            \
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);       \
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);) \
                                                                                                        \
    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = name ## _COMMAND;                            \
    ON_DEBUG(                                                                                           \
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,     \
                                             BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,           \
                                             "%hhX ", name ## _COMMAND);                                \
    )                                                                                                   \
                                                                                                        \
    Asm_arg arg = {};                                                                                   \
    CHECK_FUNC(get_arg, asm_ptr, cur_char_ptr, &arg);                                                   \
                                                                                                        \
    if (asm_ptr->byte_code_len      >= BYTE_CODE_MAX_LEN ON_DEBUG(or                                    \
        asm_ptr->text_byte_code_len >= BYTE_CODE_MAX_LEN)) {                                            \
        return BYTE_CODE_TOO_LONG;                                                                      \
    }                                                                                                   \
    asm_ptr->byte_code[asm_ptr->byte_code_len++].reg = arg.elem.reg;                                    \
    ON_DEBUG(                                                                                           \
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,     \
                                             BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,           \
                                             "%hhX\n", arg.elem.reg);                                   \
    )                                                                                                   \
                                                                                                        \
    return 0;                                                                                           \
}

#define MAKE_JUMP_COMPILATE(name)                                                                       \
static errno_t name ## _compilate(Assembler *const asm_ptr, size_t *const cur_char_ptr) {               \
    assert(asm_ptr); assert(asm_ptr->is_valid); assert(asm_ptr->code); assert(cur_char_ptr);            \
             assert(asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN); assert(asm_ptr->byte_code);       \
    ON_DEBUG(assert(asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN); assert(asm_ptr->text_byte_code);) \
                                                                                                        \
    asm_ptr->byte_code[asm_ptr->byte_code_len++].command = name ## _COMMAND;                            \
    ON_DEBUG(                                                                                           \
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,     \
                                            BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,            \
                                            "%hhX ", name ## _COMMAND);                                 \
    )                                                                                                   \
                                                                                                        \
    Asm_arg arg = {};                                                                                   \
    CHECK_FUNC(get_arg, asm_ptr, cur_char_ptr, &arg);                                                   \
                                                                                                        \
    if (asm_ptr->byte_code_len      >= BYTE_CODE_MAX_LEN ON_DEBUG(or                                    \
        asm_ptr->text_byte_code_len >= BYTE_CODE_MAX_LEN)) {                                            \
        return BYTE_CODE_TOO_LONG;                                                                      \
    }                                                                                                   \
    if (arg.elem.pos == DEFAULT_POSITION) {                                                             \
        return INVALID_LABEL;                                                                           \
    }                                                                                                   \
    asm_ptr->byte_code[asm_ptr->byte_code_len++].pos = arg.elem.pos;                                    \
    ON_DEBUG(                                                                                           \
    asm_ptr->text_byte_code_len += sprintf_s(asm_ptr->text_byte_code + asm_ptr->text_byte_code_len,     \
                                             BYTE_CODE_MAX_LEN - asm_ptr->text_byte_code_len,           \
                                             "%zu\n", arg.elem.pos);                                    \
    )                                                                                                   \
                                                                                                        \
    return 0;                                                                                           \
}

MAKE_SIMPLE_COMPILATE(HLT)

MAKE_VAL_COMPILATE(PUSH)
MAKE_REG_COMPILATE(PUSHR)
MAKE_SIMPLE_COMPILATE(POP)
MAKE_REG_COMPILATE(POPR)

MAKE_SIMPLE_COMPILATE(ADD)
MAKE_SIMPLE_COMPILATE(SUB)
MAKE_SIMPLE_COMPILATE(MLT)
MAKE_SIMPLE_COMPILATE(DIV)

MAKE_SIMPLE_COMPILATE(SQRT)
MAKE_SIMPLE_COMPILATE(POW)

MAKE_SIMPLE_COMPILATE(IN)
MAKE_SIMPLE_COMPILATE(OUT)

MAKE_JUMP_COMPILATE(JMP)
MAKE_JUMP_COMPILATE(JB)
MAKE_JUMP_COMPILATE(JBE)
MAKE_JUMP_COMPILATE(JA)
MAKE_JUMP_COMPILATE(JAE)
MAKE_JUMP_COMPILATE(JE)
MAKE_JUMP_COMPILATE(JNE)

MAKE_JUMP_COMPILATE(CALL)
MAKE_SIMPLE_COMPILATE(RET)

MAKE_REG_COMPILATE(PUSHM)
MAKE_REG_COMPILATE(POPM)

MAKE_SIMPLE_COMPILATE(DRAW)

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

    size_t cur_char  = 0,
           arg_count = 0;
    while (cur_char < cur_asm.code_len) {
        Asm_arg arg = {};
        CHECK_FUNC(get_arg, &cur_asm, &cur_char, &arg);
        if (arg.type == LAB_TYPE) {
            if (cur_asm.labels[arg.elem.lab] != DEFAULT_POSITION) {
                return INVALID_LABEL;
            }
            cur_asm.labels[arg.elem.lab] = arg_count;
        }
        else {
            ++arg_count;
        }
    }

    cur_char = 0;
    while (cur_char                   < cur_asm.code_len  and
           cur_asm.byte_code_len      < BYTE_CODE_MAX_LEN ON_DEBUG(and
           cur_asm.text_byte_code_len < BYTE_CODE_MAX_LEN)) {
        Asm_arg arg = {};
        CHECK_FUNC(get_arg, &cur_asm, &cur_char, &arg);
        if (arg.type == COMMAND_TYPE) {
            switch (arg.elem.command) {
                case HLT_COMMAND: //TODO - possible macros
                    CHECK_FUNC(HLT_compilate, &cur_asm, &cur_char);
                    break;

                case PUSH_COMMAND:
                    CHECK_FUNC(PUSH_compilate, &cur_asm, &cur_char);
                    break;

                case PUSHR_COMMAND:
                    CHECK_FUNC(PUSHR_compilate, &cur_asm, &cur_char);
                    break;

                case POP_COMMAND:
                    CHECK_FUNC(POP_compilate, &cur_asm, &cur_char);
                    break;

                case POPR_COMMAND:
                    CHECK_FUNC(POPR_compilate, &cur_asm, &cur_char);
                    break;

                case ADD_COMMAND:
                    CHECK_FUNC(ADD_compilate, &cur_asm, &cur_char);
                    break;

                case SUB_COMMAND:
                    CHECK_FUNC(SUB_compilate, &cur_asm, &cur_char);
                    break;

                case MLT_COMMAND:
                    CHECK_FUNC(MLT_compilate, &cur_asm, &cur_char);
                    break;

                case DIV_COMMAND:
                    CHECK_FUNC(DIV_compilate, &cur_asm, &cur_char);
                    break;

                case SQRT_COMMAND:
                    CHECK_FUNC(SQRT_compilate, &cur_asm, &cur_char);
                    break;

                case POW_COMMAND:
                    CHECK_FUNC(POW_compilate, &cur_asm, &cur_char);
                    break;

                case IN_COMMAND:
                    CHECK_FUNC(IN_compilate, &cur_asm, &cur_char);
                    break;

                case OUT_COMMAND:
                    CHECK_FUNC(OUT_compilate, &cur_asm, &cur_char);
                    break;

                case JMP_COMMAND:
                    CHECK_FUNC(JMP_compilate, &cur_asm, &cur_char);
                    break;

                case JB_COMMAND:
                    CHECK_FUNC(JB_compilate, &cur_asm, &cur_char);
                    break;

                case JBE_COMMAND:
                    CHECK_FUNC(JBE_compilate, &cur_asm, &cur_char);
                    break;

                case JA_COMMAND:
                    CHECK_FUNC(JA_compilate, &cur_asm, &cur_char);
                    break;

                case JAE_COMMAND:
                    CHECK_FUNC(JAE_compilate, &cur_asm, &cur_char);
                    break;

                case JE_COMMAND:
                    CHECK_FUNC(JE_compilate, &cur_asm, &cur_char);
                    break;

                case JNE_COMMAND:
                    CHECK_FUNC(JNE_compilate, &cur_asm, &cur_char);
                    break;

                case CALL_COMMAND:
                    CHECK_FUNC(CALL_compilate, &cur_asm, &cur_char);
                    break;

                case RET_COMMAND:
                    CHECK_FUNC(RET_compilate, &cur_asm, &cur_char);
                    break;

                case PUSHM_COMMAND:
                    CHECK_FUNC(PUSHM_compilate, &cur_asm, &cur_char);
                    break;

                case POPM_COMMAND:
                    CHECK_FUNC(POPM_compilate, &cur_asm, &cur_char);
                    break;

                case DRAW_COMMAND:
                    CHECK_FUNC(DRAW_compilate, &cur_asm, &cur_char);
                    break;

                case __ASM_COMMAND_COUNT:
                default:
                    CLEAR_RESOURCES();
                    return UNKNOWN_ASM_COMMAND;
            }
        }
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
