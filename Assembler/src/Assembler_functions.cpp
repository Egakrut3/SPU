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
    CHECK_FUNC(My_sscanf_s, 1, asm_ptr->code + *cur_char_ptr, "%s %zn",
                               cur_command, ASM_COMMAND_MAX_LEN + 1, &extra_len);
    *cur_char_ptr += extra_len;

    #undef HANDLE_COMMAND
    #define HANDLE_COMMAND(name)                    \
    if (!strcmp(cur_command, #name)) {              \
        arg_ptr->elem.command = name ## _COMMAND;   \
                                                    \
        arg_ptr->type = COMMAND_TYPE;               \
        return 0;                                   \
    }

    //TODO -
    #include "Command_list.h"

    return UNKNOWN_ASM_COMMAND;
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

//TODO - possible include
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

static errno_t set_labels(Assembler *const asm_ptr) {
    size_t cur_char  = 0,
           arg_count = 0;
    while (cur_char < asm_ptr->code_len) {
        Asm_arg arg = {};
        CHECK_FUNC(get_arg, asm_ptr, &cur_char, &arg);
        if (arg.type == LAB_TYPE) {
            if (asm_ptr->labels[arg.elem.lab] != DEFAULT_POSITION) {
                return INVALID_LABEL;
            }
            asm_ptr->labels[arg.elem.lab] = arg_count;
        }
        else {
            ++arg_count;
        }
    }

    return 0;
}

static errno_t parse_commands(Assembler *const asm_ptr) {
    size_t cur_char = 0;
    while (cur_char                    < asm_ptr->code_len  and
           asm_ptr->byte_code_len      < BYTE_CODE_MAX_LEN ON_DEBUG(and
           asm_ptr->text_byte_code_len < BYTE_CODE_MAX_LEN)) { //TODO -
        Asm_arg arg = {};
        CHECK_FUNC(get_arg, asm_ptr, &cur_char, &arg);
        if (arg.type == COMMAND_TYPE) {

            #undef HANDLE_COMMAND
            #define HANDLE_COMMAND(name)                            \
            case name ## _COMMAND:                                  \
                CHECK_FUNC(name ## _compilate, asm_ptr, &cur_char); \
                break;

            switch (arg.elem.command) {
                //TODO -
                #include "Command_list.h"

                case __ASM_COMMAND_COUNT:
                default:
                    return UNKNOWN_ASM_COMMAND;
            }
        }
    }

    if (cur_char < asm_ptr->code_len) {
        return BYTE_CODE_TOO_LONG;
    }

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

    CHECK_FUNC(set_labels, &cur_asm);
    CHECK_FUNC(parse_commands, &cur_asm);

    CHECK_FUNC(My_fwrite, &cur_asm.byte_code_len, sizeof(cur_asm.byte_code_len), 1,
                          byte_code_stream);
    CHECK_FUNC(My_fwrite, cur_asm.byte_code, sizeof(*cur_asm.byte_code), cur_asm.byte_code_len,
                          byte_code_stream);
    ON_DEBUG(fprintf_s(text_byte_code_stream, "%s", cur_asm.text_byte_code);)

    CLEAR_RESOURCES();
    return 0;
}
