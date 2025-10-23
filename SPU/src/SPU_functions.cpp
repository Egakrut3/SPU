#include "SPU.h"
#include <math.h>

#undef FINAL_CODE
#define FINAL_CODE

static errno_t HLT_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    ON_DEBUG(fprintf_s(stderr, "Finishing execution\n");)

    return 0;
}

static errno_t PUSH_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    if (*IC_ptr >= SPU_ptr->byte_code_len) {
        return NOT_ENOUGH_ARGUMENTS;
    }
    stack_elem_t x = SPU_ptr->byte_code[(*IC_ptr)++].val;
    ON_DEBUG(fprintf_s(stderr, "Trying to PUSH " STACK_ELEM_FRM "\n", x);)
    CHECK_FUNC(My_stack_push, &SPU_ptr->calc_stack, x);

    return 0;
}

static errno_t PUSHR_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    if (*IC_ptr >= SPU_ptr->byte_code_len) {
        return NOT_ENOUGH_ARGUMENTS;
    }
    byte_elem_t reg = SPU_ptr->byte_code[(*IC_ptr)++].reg;
    if (reg >= REGS_NUM) {
        return INVALID_REGISTER;
    }
    ON_DEBUG(fprintf_s(stderr, "Trying to PUSHR r%hhu(" STACK_ELEM_FRM ")\n",
                               reg, SPU_ptr->regs[reg]);)
    CHECK_FUNC(My_stack_push, &SPU_ptr->calc_stack, SPU_ptr->regs[reg]);

    return 0;
}

static errno_t POP_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    ON_DEBUG(fprintf_s(stderr, "Trying to POP\n");)
    CHECK_FUNC(My_stack_pop, &SPU_ptr->calc_stack, nullptr);

    return 0;
}

static errno_t POPR_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    if (*IC_ptr >= SPU_ptr->byte_code_len) {
        return NOT_ENOUGH_ARGUMENTS;
    }
    byte_elem_t reg = SPU_ptr->byte_code[(*IC_ptr)++].reg;
    if (reg >= REGS_NUM) {
        return INVALID_REGISTER;
    }
    ON_DEBUG(fprintf_s(stderr, "Trying to POPR r%hhu\n", reg);)
    CHECK_FUNC(My_stack_pop, &SPU_ptr->calc_stack, &SPU_ptr->regs[reg]);

    return 0;
}

#define MAKE_SIMPLE_BIN_EXECUTE(name, operation)                                                    \
static errno_t name ## _execute(SPU *const SPU_ptr, size_t *const IC_ptr) {                         \
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);                     \
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)                                                      \
                                                                                                    \
    stack_elem_t x = {},                                                                            \
                 y = {};                                                                            \
    CHECK_FUNC(My_stack_pop, &SPU_ptr->calc_stack, &y);                                             \
    CHECK_FUNC(My_stack_pop, &SPU_ptr->calc_stack, &x);                                             \
    ON_DEBUG(fprintf_s(stderr, "Trying to " #name " " STACK_ELEM_FRM " " STACK_ELEM_FRM "\n",       \
                               x, y);)                                                              \
    CHECK_FUNC(My_stack_push, &SPU_ptr->calc_stack, x operation y);                                 \
                                                                                                    \
    return 0;                                                                                       \
}

MAKE_SIMPLE_BIN_EXECUTE(ADD, +)
MAKE_SIMPLE_BIN_EXECUTE(SUB, -)
MAKE_SIMPLE_BIN_EXECUTE(MLT, *)
MAKE_SIMPLE_BIN_EXECUTE(DIV, /)

static errno_t SQRT_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    stack_elem_t x = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->calc_stack, &x);
    ON_DEBUG(fprintf_s(stderr, "Trying to SQRT " STACK_ELEM_FRM "\n", x);)
    CHECK_FUNC(My_stack_push, &SPU_ptr->calc_stack, sqrt(x));

    return 0;
}

static errno_t POW_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->calc_stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->calc_stack, &x);
    ON_DEBUG(fprintf_s(stderr, "Trying to POW " STACK_ELEM_FRM " " STACK_ELEM_FRM "\n", x, y);)
    CHECK_FUNC(My_stack_push, &SPU_ptr->calc_stack, pow(x, y));

    return 0;
}

static errno_t IN_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    stack_elem_t x = {};
    fprintf_s(stderr, "Trying to IN\n");
    CHECK_FUNC(My_scanf_s, 1, STACK_ELEM_FRM, &x);
    CHECK_FUNC(My_stack_push, &SPU_ptr->calc_stack, x);

    return 0;
}

static errno_t OUT_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    stack_elem_t x = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->calc_stack, &x);
    fprintf_s(stderr, "Trying to OUT " STACK_ELEM_FRM "\n", x);
    colored_printf(MAGENTA, BLACK, STACK_ELEM_FRM "\n", x);
    getchar();

    return 0;
}

static errno_t JMP_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    if (*IC_ptr >= SPU_ptr->byte_code_len) {
        return NOT_ENOUGH_ARGUMENTS;
    }
    size_t pos = SPU_ptr->byte_code[(*IC_ptr)++].pos;
    if (pos >= SPU_ptr->byte_code_len) {
        return INVALID_POSITION;
    }
    fprintf_s(stderr, "Trying to JMP %zu\n", pos);
    *IC_ptr = pos;

    return 0;
}

#define MAKE_COND_JUMP_EXECUTE(name, operation)                                                     \
static errno_t name ## _execute(SPU *const SPU_ptr, size_t *const IC_ptr) {                         \
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);                     \
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)                                                      \
                                                                                                    \
    if (*IC_ptr >= SPU_ptr->byte_code_len) {                                                        \
        return NOT_ENOUGH_ARGUMENTS;                                                                \
    }                                                                                               \
    size_t pos = SPU_ptr->byte_code[(*IC_ptr)++].pos;                                               \
    if (pos >= SPU_ptr->byte_code_len) {                                                            \
        return INVALID_POSITION;                                                                    \
    }                                                                                               \
    stack_elem_t x = {},                                                                            \
                 y = {};                                                                            \
    CHECK_FUNC(My_stack_pop, &SPU_ptr->calc_stack, &y);                                             \
    CHECK_FUNC(My_stack_pop, &SPU_ptr->calc_stack, &x);                                             \
    ON_DEBUG(fprintf_s(stderr, "Trying to " #name " %zu "                                           \
                               STACK_ELEM_FRM " " #operation " " STACK_ELEM_FRM "\n",               \
                               pos, x, y);)                                                         \
    if (x operation y) {                                                                            \
        *IC_ptr = pos;                                                                              \
    }                                                                                               \
                                                                                                    \
    return 0;                                                                                       \
}

MAKE_COND_JUMP_EXECUTE(JB, <)
MAKE_COND_JUMP_EXECUTE(JBE, <=)
MAKE_COND_JUMP_EXECUTE(JA, >)
MAKE_COND_JUMP_EXECUTE(JAE, >=)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
MAKE_COND_JUMP_EXECUTE(JE, ==)
MAKE_COND_JUMP_EXECUTE(JNE, !=)
#pragma GCC diagnostic pop

static errno_t CALL_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    if (*IC_ptr >= SPU_ptr->byte_code_len) {
        return NOT_ENOUGH_ARGUMENTS;
    }
    size_t pos = SPU_ptr->byte_code[(*IC_ptr)++].pos;
    if (pos >= SPU_ptr->byte_code_len) {
        return INVALID_POSITION;
    }
    fprintf_s(stderr, "Trying to CALL %zu with previous pos = %zu\n", pos, *IC_ptr);
    CHECK_FUNC(My_stack_push, &SPU_ptr->call_stack, (stack_elem_t)(*IC_ptr));
    *IC_ptr = pos;

    return 0;
}

static errno_t RET_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    stack_elem_t pos = {};
    fprintf_s(stderr, "Trying to RET\n");
    CHECK_FUNC(My_stack_pop, &SPU_ptr->call_stack, &pos);
    *IC_ptr = (size_t)pos;

    return 0;
}

static errno_t PUSHM_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    if (*IC_ptr >= SPU_ptr->byte_code_len) {
        return NOT_ENOUGH_ARGUMENTS;
    }
    byte_elem_t reg = SPU_ptr->byte_code[(*IC_ptr)++].reg;
    if (reg >= REGS_NUM) {
        return INVALID_REGISTER;
    }
    size_t index = (size_t)SPU_ptr->regs[reg];
    if (index >= SPU_MEM_SIZE_SQRT * SPU_MEM_SIZE_SQRT) {
        return INVALID_INDEX;
    }
    fprintf_s(stderr, "Trying to PUSHM r%hhu with index = %zu\n", reg, index);
    CHECK_FUNC(My_stack_push, &SPU_ptr->calc_stack, SPU_ptr->memory[index]);

    return 0;
}

static errno_t POPM_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    if (*IC_ptr >= SPU_ptr->byte_code_len) {
        return NOT_ENOUGH_ARGUMENTS;
    }
    byte_elem_t reg = SPU_ptr->byte_code[(*IC_ptr)++].reg;
    if (reg >= REGS_NUM) {
        return INVALID_REGISTER;
    }
    size_t index = (size_t)SPU_ptr->regs[reg];
    if (index >= SPU_MEM_SIZE_SQRT * SPU_MEM_SIZE_SQRT) {
        return INVALID_INDEX;
    }
    fprintf_s(stderr, "Trying to POPM r%hhu with index = %zu\n", reg, index);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->calc_stack, &SPU_ptr->memory[index]);

    return 0;
}

static errno_t DRAW_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)


    fprintf_s(stderr, "Trying to DRAW\n");
    for (size_t i = 0; i < SPU_MEM_SIZE_SQRT; ++i) {
        for (size_t j = 0; j < SPU_MEM_SIZE_SQRT; ++j) {
            printf_s("%c", (char)SPU_ptr->memory[i * SPU_MEM_SIZE_SQRT + j]);
        }
        printf_s("\n");
    }

    return 0;
}

errno_t SPU_execute(SPU *const SPU_ptr) {
    assert(SPU_ptr);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    size_t IC = 0;
    while (IC < SPU_ptr->byte_code_len) {
        byte_elem_t const cur_command = SPU_ptr->byte_code[IC++].command;
        if (cur_command >= __ASM_COMMAND_COUNT) {
            return UNKNOWN_ASM_COMMAND;
        }

        #undef HANDLE_COMMAND
        #define HANDLE_COMMAND(name)                                \
        case name ## _COMMAND:                                      \
                CHECK_FUNC(name ## _execute, SPU_ptr, &IC);   \
                break;

        switch(cur_command) {
            //TODO - 
            #include "Command_list.h"

            case __ASM_COMMAND_COUNT:
            default:
                abort(); //TODO -
        }
        SPU_ptr->hash_val = SPU_hash(SPU_ptr);

        if (cur_command == HLT_COMMAND) {
            if (SPU_ptr->calc_stack.size) { return STACK_NOT_EMPTY_AFTER_EXECUTION; }
            else                          { return 0; }
        }
    }

    return NO_HLT_COMMAND_REACHED;
}
