#include "SPU.h"
#include <math.h>

errno_t SPU_execute(SPU *const SPU_ptr) {
    assert(SPU_ptr);


    stack_elem_t op1 = {},
                 op2 = {};
#undef FINAL_CODE
#define FINAL_CODE

    for (size_t i = 0; i < SPU_ptr->byte_code_len; ++i) {
        //fprintf_s(stderr, "i = %zu\n", i);
        switch (SPU_ptr->byte_code[i]) {
            case HLT_COMMAND:
                CLEAR_RESOURCES();
                if (SPU_ptr->stack.size) { return STACK_NOT_EMPTY_AFTER_EXECUTION; }
                else { return 0; }

            case PUSH_COMMAND:
                i = get_assembler_aligned(i + 1);
                if (i < SPU_ptr->byte_code_len) {
                    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, *(stack_elem_t *)(SPU_ptr->byte_code + i));
                    i += 7;
                    break;
                }
                else {
                    CLEAR_RESOURCES();
                    return INCOMPLETE_ARGUMENT;
                }

            case ADD_COMMAND:
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op2);
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op1);
                CHECK_FUNC(My_stack_push, &SPU_ptr->stack, op1 + op2);
                break;

            case SUB_COMMAND:
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op2);
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op1);
                CHECK_FUNC(My_stack_push, &SPU_ptr->stack, op1 - op2);
                break;

            case MLT_COMMAND:
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op2);
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op1);
                CHECK_FUNC(My_stack_push, &SPU_ptr->stack, op1 * op2);
                break;

            case DIV_COMMAND:
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op2);
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op1);
                if (op2 == 0) { CLEAR_RESOURCES(); return EDOM; }
                CHECK_FUNC(My_stack_push, &SPU_ptr->stack, op1 / op2);
                break;

            case SQRT_COMMAND:
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op1);
                if (op1 < 0) { CLEAR_RESOURCES(); return EDOM; }
                CHECK_FUNC(My_stack_push, &SPU_ptr->stack, sqrt(op1));
                break;

            case POW_COMMAND:
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op2);
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op1);
                CHECK_FUNC(My_stack_push, &SPU_ptr->stack, pow(op1, op2)); //TODO - how to check
                break;

            case OUT_COMMAND:
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op1);
                printf_s(stack_elem_frm, op1);
                printf_s("\n");
                break;

            default:
                CLEAR_RESOURCES();
                return UNKNOWN_COMMAND;
        }
    }

    CLEAR_RESOURCES();
    return NO_HLT_COMMAND;
}
