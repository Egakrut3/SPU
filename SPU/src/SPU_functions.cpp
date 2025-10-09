#include "SPU.h"
#include <math.h>

static errno_t HLT_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == HLT_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    fprintf_s(stderr, "Finishing execution\n");

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t PUSH_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == PUSH_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    *IC_ptr = get_assembler_aligned(*IC_ptr + 1);
    if (*IC_ptr + sizeof(stack_elem_t) > SPU_ptr->byte_code_len) {
        CLEAR_RESOURCES();
        return INCOMPLETE_ARGUMENT;
    }

    stack_elem_t x = *(stack_elem_t *)(SPU_ptr->byte_code + *IC_ptr);
    fprintf_s(stderr, "Trying to PUSH ");
    fprintf_s(stderr, stack_elem_frm, x);
    fprintf_s(stderr, "\n");
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, x);

    *IC_ptr += sizeof(stack_elem_t);
    CLEAR_RESOURCES();
    return 0;
}

static errno_t PUSHR_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == PUSHR_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    ++*IC_ptr;
    if (*IC_ptr + 1 > SPU_ptr->byte_code_len) {
        CLEAR_RESOURCES();
        return INCOMPLETE_ARGUMENT;
    }

    byte_elem_t reg = SPU_ptr->byte_code[*IC_ptr];
    fprintf_s(stderr, "Trying to PUSHR r%hhu\n", reg);
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, SPU_ptr->regs[reg]);

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t POP_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == POP_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    fprintf_s(stderr, "Trying to POP\n");
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, nullptr);

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t POPR_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == POPR_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    ++*IC_ptr;
    if (*IC_ptr + 1 > SPU_ptr->byte_code_len) {
        CLEAR_RESOURCES();
        return INCOMPLETE_ARGUMENT;
    }

    byte_elem_t reg = SPU_ptr->byte_code[*IC_ptr];
    fprintf_s(stderr, "Trying to POPR r%hhu\n", reg);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &SPU_ptr->regs[reg]);

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t ADD_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == ADD_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to ADD ");
    fprintf_s(stderr, stack_elem_frm, x);
    fprintf_s(stderr, " ");
    fprintf_s(stderr, stack_elem_frm, y);
    fprintf_s(stderr, "\n");
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, x + y);

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t SUB_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == SUB_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to SUB ");
    fprintf_s(stderr, stack_elem_frm, x);
    fprintf_s(stderr, " ");
    fprintf_s(stderr, stack_elem_frm, y);
    fprintf_s(stderr, "\n");
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, x - y);

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t MLT_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == MLT_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to MLT ");
    fprintf_s(stderr, stack_elem_frm, x);
    fprintf_s(stderr, " ");
    fprintf_s(stderr, stack_elem_frm, y);
    fprintf_s(stderr, "\n");
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, x * y);

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t DIV_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == DIV_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to DIV ");
    fprintf_s(stderr, stack_elem_frm, x);
    fprintf_s(stderr, " ");
    fprintf_s(stderr, stack_elem_frm, y);
    fprintf_s(stderr, "\n");
    if (y == 0) { CLEAR_RESOURCES(); return EDOM; }
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, x / y);

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t SQRT_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == SQRT_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    stack_elem_t x = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to SQRT ");
    fprintf_s(stderr, stack_elem_frm, x);
    fprintf_s(stderr, "\n");
    if (x < 0) { CLEAR_RESOURCES(); return EDOM; }
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, sqrt(x));

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t POW_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == POW_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to POW ");
    fprintf_s(stderr, stack_elem_frm, x);
    fprintf_s(stderr, " ");
    fprintf_s(stderr, stack_elem_frm, y);
    fprintf_s(stderr, "\n");
    //TODO - how to check
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, pow(x, y));

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t IN_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == IN_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    stack_elem_t x = {};
    fprintf_s(stderr, "Trying to IN\n");
    CHECK_FUNC(My_scanf_s, 1, stack_elem_frm, &x);
    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, x);

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t OUT_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == OUT_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    stack_elem_t x = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);
    fprintf_s(stderr, "Trying to OUT ");
    fprintf_s(stderr, stack_elem_frm, x);
    fprintf_s(stderr, "\n");
    colored_printf(YELLOW, BLACK, stack_elem_frm, x);
    colored_printf(YELLOW, BLACK, "\n");
    getchar();

    ++*IC_ptr;
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JMP_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == JMP_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    ++*IC_ptr;
    if (*IC_ptr + 1 > SPU_ptr->byte_code_len) {
        CLEAR_RESOURCES();
        return INCOMPLETE_ARGUMENT;
    }

    fprintf_s(stderr, "Trying to JMP %hhu\n", SPU_ptr->byte_code[*IC_ptr]);
    *IC_ptr = SPU_ptr->byte_code[*IC_ptr];
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JB_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == JB_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);

    ++*IC_ptr;
    if (*IC_ptr + 1 > SPU_ptr->byte_code_len) {
        CLEAR_RESOURCES();
        return INCOMPLETE_ARGUMENT;
    }

    fprintf_s(stderr, "Trying to JB %hhu\n", SPU_ptr->byte_code[*IC_ptr]);
    if (x < y) {
        *IC_ptr = SPU_ptr->byte_code[*IC_ptr];
    }
    else {
        ++*IC_ptr;
    }
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JBE_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == JBE_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);

    ++*IC_ptr;
    if (*IC_ptr + 1 > SPU_ptr->byte_code_len) {
        CLEAR_RESOURCES();
        return INCOMPLETE_ARGUMENT;
    }

    fprintf_s(stderr, "Trying to JBE %hhu\n", SPU_ptr->byte_code[*IC_ptr]);
    if (x <= y) {
        *IC_ptr = SPU_ptr->byte_code[*IC_ptr];
    }
    else {
        ++*IC_ptr;
    }
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JA_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == JA_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);

    ++*IC_ptr;
    if (*IC_ptr + 1 > SPU_ptr->byte_code_len) {
        CLEAR_RESOURCES();
        return INCOMPLETE_ARGUMENT;
    }

    fprintf_s(stderr, "Trying to JA %hhu\n", SPU_ptr->byte_code[*IC_ptr]);
    if (x > y) {
        *IC_ptr = SPU_ptr->byte_code[*IC_ptr];
    }
    else {
        ++*IC_ptr;
    }
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JAE_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == JAE_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);

    ++*IC_ptr;
    if (*IC_ptr + 1 > SPU_ptr->byte_code_len) {
        CLEAR_RESOURCES();
        return INCOMPLETE_ARGUMENT;
    }

    fprintf_s(stderr, "Trying to JAE %hhu\n", SPU_ptr->byte_code[*IC_ptr]);
    if (x >= y) {
        *IC_ptr = SPU_ptr->byte_code[*IC_ptr];
    }
    else {
        ++*IC_ptr;
    }
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JE_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == JE_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);

    ++*IC_ptr;
    if (*IC_ptr + 1 > SPU_ptr->byte_code_len) {
        CLEAR_RESOURCES();
        return INCOMPLETE_ARGUMENT;
    }

    fprintf_s(stderr, "Trying to JE %hhu\n", SPU_ptr->byte_code[*IC_ptr]);
    if (x == y) {
        *IC_ptr = SPU_ptr->byte_code[*IC_ptr];
    }
    else {
        ++*IC_ptr;
    }
    CLEAR_RESOURCES();
    return 0;
}

static errno_t JNE_execute(size_t *const IC_ptr, SPU *const SPU_ptr) {
    assert(IC_ptr); assert(SPU_ptr); assert(SPU_ptr->byte_code[*IC_ptr] == JNE_COMMAND);

#undef FINAL_CODE
#define FINAL_CODE

    stack_elem_t x = {},
                 y = {};
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &y);
    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &x);

    ++*IC_ptr;
    if (*IC_ptr + 1 > SPU_ptr->byte_code_len) {
        CLEAR_RESOURCES();
        return INCOMPLETE_ARGUMENT;
    }

    fprintf_s(stderr, "Trying to JNE %hhu\n", SPU_ptr->byte_code[*IC_ptr]);
    if (x != y) {
        *IC_ptr = SPU_ptr->byte_code[*IC_ptr];
    }
    else {
        ++*IC_ptr;
    }
    CLEAR_RESOURCES();
    return 0;
}

static errno_t (*const execute_functions[__ASM_COMMAND_COUNT])(size_t *, SPU *) = {
    &HLT_execute,
    &PUSH_execute,
    &PUSHR_execute,
    &POP_execute,
    &POPR_execute,
    &ADD_execute,
    &SUB_execute,
    &MLT_execute,
    &DIV_execute,
    &SQRT_execute,
    &POW_execute,
    &IN_execute,
    &OUT_execute,
    &JMP_execute,
    &JB_execute,
    &JBE_execute,
    &JA_execute,
    &JAE_execute,
    &JE_execute,
    &JNE_execute,
};

errno_t SPU_execute(SPU *const SPU_ptr) {
    assert(SPU_ptr);

#undef FINAL_CODE
#define FINAL_CODE

    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr));

    size_t IC = 0;
    while (IC < SPU_ptr->byte_code_len) {
        byte_elem_t const cur_command = SPU_ptr->byte_code[IC];
        if (cur_command >= __ASM_COMMAND_COUNT) { CLEAR_RESOURCES(); return UNKNOWN_COMMAND; }

        CHECK_FUNC(execute_functions[cur_command], &IC, SPU_ptr);

        if (cur_command == HLT_COMMAND) {
            if (SPU_ptr->stack.size) { CLEAR_RESOURCES(); return STACK_NOT_EMPTY_AFTER_EXECUTION; }
            else                     { CLEAR_RESOURCES(); return 0; }
        }
    }

    CLEAR_RESOURCES();
    return NO_HLT_COMMAND_REACHED;
}
