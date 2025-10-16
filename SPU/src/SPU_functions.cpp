#include "SPU.h"
#include <math.h>

#undef FINAL_CODE
#define FINAL_CODE

static errno_t HLT_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    fprintf_s(stderr, "Finishing execution\n");

    return 0;
}

static errno_t PUSH_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    if (*IC_ptr >= SPU_ptr->byte_code_len) {
        return NOT_ENOUGH_ARGUMENTS;
    }
    stack_elem_t x = SPU_ptr->byte_code[(*IC_ptr)++].val;
    fprintf_s(stderr, "Trying to PUSH " STACK_ELEM_FRM "\n", x);
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, x);

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
    fprintf_s(stderr, "Trying to PUSHR r%hhu\n", reg);
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, SPU_ptr->regs[reg]);

    return 0;
}

static errno_t POP_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    fprintf_s(stderr, "Trying to POP\n");
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, nullptr);

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
    fprintf_s(stderr, "Trying to POPR r%hhu\n", reg);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &SPU_ptr->regs[reg]);

    return 0;
}

static errno_t ADD_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to ADD " STACK_ELEM_FRM " " STACK_ELEM_FRM "\n", x, y);
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, x + y);

    return 0;
}

static errno_t SUB_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to SUB " STACK_ELEM_FRM " " STACK_ELEM_FRM "\n", x, y);
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, x - y);

    return 0;
}

static errno_t MLT_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to MLT " STACK_ELEM_FRM " " STACK_ELEM_FRM "\n", x, y);
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, x * y);

    return 0;
}

static errno_t DIV_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to DIV " STACK_ELEM_FRM " " STACK_ELEM_FRM "\n", x, y);
    if (y == 0) { return EDOM; }
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, x / y);

    return 0;
}

static errno_t SQRT_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    stack_elem_t x = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to SQRT " STACK_ELEM_FRM "\n", x);
    if (x < 0) { return EDOM; }
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, sqrt(x));

    return 0;
}

static errno_t POW_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to POW " STACK_ELEM_FRM " " STACK_ELEM_FRM "\n", x, y);
    //TODO - make check
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, pow(x, y));

    return 0;
}

static errno_t IN_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    stack_elem_t x = {};
    fprintf_s(stderr, "Trying to IN\n");
    CHECK_FUNC(My_scanf_s, 1, STACK_ELEM_FRM, &x);
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, x);

    return 0;
}

static errno_t OUT_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    stack_elem_t x = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
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

static errno_t JB_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    if (*IC_ptr >= SPU_ptr->byte_code_len) {
        return NOT_ENOUGH_ARGUMENTS;
    }
    size_t pos = SPU_ptr->byte_code[(*IC_ptr)++].pos;
    if (pos >= SPU_ptr->byte_code_len) {
        return INVALID_POSITION;
    }
    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to JB %zu\n", pos);
    if (x < y) {
        *IC_ptr = pos;
    }

    return 0;
}

static errno_t JBE_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    if (*IC_ptr >= SPU_ptr->byte_code_len) {
        return NOT_ENOUGH_ARGUMENTS;
    }
    size_t pos = SPU_ptr->byte_code[(*IC_ptr)++].pos;
    if (pos >= SPU_ptr->byte_code_len) {
        return INVALID_POSITION;
    }
    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to JBE %zu\n", pos);
    if (x <= y) {
        *IC_ptr = pos;
    }

    return 0;
}

static errno_t JA_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    if (*IC_ptr >= SPU_ptr->byte_code_len) {
        return NOT_ENOUGH_ARGUMENTS;
    }
    size_t pos = SPU_ptr->byte_code[(*IC_ptr)++].pos;
    if (pos >= SPU_ptr->byte_code_len) {
        return INVALID_POSITION;
    }
    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to JA %zu\n", pos);
    if (x > y) {
        *IC_ptr = pos;
    }

    return 0;
}

static errno_t JAE_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    if (*IC_ptr >= SPU_ptr->byte_code_len) {
        return NOT_ENOUGH_ARGUMENTS;
    }
    size_t pos = SPU_ptr->byte_code[(*IC_ptr)++].pos;
    if (pos >= SPU_ptr->byte_code_len) {
        return INVALID_POSITION;
    }
    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to JAE %zu\n", pos);
    if (x >= y) {
        *IC_ptr = pos;
    }

    return 0;
}

static errno_t JE_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    if (*IC_ptr >= SPU_ptr->byte_code_len) {
        return NOT_ENOUGH_ARGUMENTS;
    }
    size_t pos = SPU_ptr->byte_code[(*IC_ptr)++].pos;
    if (pos >= SPU_ptr->byte_code_len) {
        return INVALID_POSITION;
    }
    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to JB %zu\n", pos);
    if (x == y) {
        *IC_ptr = pos;
    }

    return 0;
}

static errno_t JNE_execute(SPU *const SPU_ptr, size_t *const IC_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(*IC_ptr <= SPU_ptr->byte_code_len);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    if (*IC_ptr >= SPU_ptr->byte_code_len) {
        return NOT_ENOUGH_ARGUMENTS;
    }
    size_t pos = SPU_ptr->byte_code[(*IC_ptr)++].pos;
    if (pos >= SPU_ptr->byte_code_len) {
        return INVALID_POSITION;
    }
    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to JB %zu\n", pos);
    if (x != y) {
        *IC_ptr = pos;
    }

    return 0;
}

errno_t SPU_execute(SPU *const SPU_ptr) {
    assert(SPU_ptr);
    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr);)

    size_t IC = 0;
    while (IC < SPU_ptr->byte_code_len) {
        byte_elem_t const cur_command = SPU_ptr->byte_code[IC++].command;
        if (cur_command >= __ASM_COMMAND_COUNT) { return UNKNOWN_ASM_COMMAND; }

        switch (cur_command) {
            case HLT_COMMAND:
                CHECK_FUNC(HLT_execute, SPU_ptr, &IC);
                break;

            case PUSH_COMMAND:
                CHECK_FUNC(PUSH_execute, SPU_ptr, &IC);
                break;

            case PUSHR_COMMAND:
                CHECK_FUNC(PUSHR_execute, SPU_ptr, &IC);
                break;

            case POP_COMMAND:
                CHECK_FUNC(POP_execute, SPU_ptr, &IC);
                break;

            case POPR_COMMAND:
                CHECK_FUNC(POPR_execute, SPU_ptr, &IC);
                break;

            case ADD_COMMAND:
                CHECK_FUNC(ADD_execute, SPU_ptr, &IC);
                break;

            case SUB_COMMAND:
                CHECK_FUNC(SUB_execute, SPU_ptr, &IC);
                break;

            case MLT_COMMAND:
                CHECK_FUNC(MLT_execute, SPU_ptr, &IC);
                break;

            case DIV_COMMAND:
                CHECK_FUNC(DIV_execute, SPU_ptr, &IC);
                break;

            case SQRT_COMMAND:
                CHECK_FUNC(SQRT_execute, SPU_ptr, &IC);
                break;

            case POW_COMMAND:
                CHECK_FUNC(POW_execute, SPU_ptr, &IC);
                break;

            case IN_COMMAND:
                CHECK_FUNC(IN_execute, SPU_ptr, &IC);
                break;

            case OUT_COMMAND:
                CHECK_FUNC(OUT_execute, SPU_ptr, &IC);
                break;

            case JMP_COMMAND:
                CHECK_FUNC(JMP_execute, SPU_ptr, &IC);
                break;

            case JB_COMMAND:
                CHECK_FUNC(JB_execute, SPU_ptr, &IC);
                break;

            case JBE_COMMAND:
                CHECK_FUNC(JBE_execute, SPU_ptr, &IC);
                break;

            case JA_COMMAND:
                CHECK_FUNC(JA_execute, SPU_ptr, &IC);
                break;

            case JAE_COMMAND:
                CHECK_FUNC(JAE_execute, SPU_ptr, &IC);
                break;

            case JE_COMMAND:
                CHECK_FUNC(JE_execute, SPU_ptr, &IC);
                break;

            case JNE_COMMAND:
                CHECK_FUNC(JNE_execute, SPU_ptr, &IC);
                break;

            case __ASM_COMMAND_COUNT:
            default:
                return UNKNOWN_ASM_COMMAND;
        }

        SPU_ptr->hash_val = SPU_hash(SPU_ptr);

        if (cur_command == HLT_COMMAND) {
            if (SPU_ptr->stack.size) { return STACK_NOT_EMPTY_AFTER_EXECUTION; }
            else                     { return 0; }
        }
    }

    return NO_HLT_COMMAND_REACHED;
}
