#include "SPU.h"
#include <math.h>

errno_t SPU_execute(SPU *const SPU_ptr) {
    assert(SPU_ptr);


    stack_elem_t op1 = {},
                 op2 = {};
    byte_elem_t reg1 = {};
#undef FINAL_CODE
#define FINAL_CODE

    for (size_t i = 0; i < SPU_ptr->byte_code_len; ++i) {
        //fprintf_s(stderr, "i = %zu\n", i);
        switch (SPU_ptr->byte_code[i]) {
            case HLT_COMMAND:
                CLEAR_RESOURCES();
                fprintf_s(stderr, "Finishing execution\n");
                if (SPU_ptr->stack.size) { return STACK_NOT_EMPTY_AFTER_EXECUTION; }
                else { return 0; }

            case PUSH_COMMAND:
                i = get_assembler_aligned(i + 1);
                if (i < SPU_ptr->byte_code_len) {
                    op1 = *(stack_elem_t *)(SPU_ptr->byte_code + i);
                    fprintf_s(stderr, "Trying to PUSH ");
                    fprintf_s(stderr, stack_elem_frm, op1);
                    fprintf_s(stderr, "\n");
                    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, op1);
                    i += 7;
                    break;
                }
                else {
                    CLEAR_RESOURCES();
                    return INCOMPLETE_ARGUMENT;
                }

            case PUSHR_COMMAND:
                ++i;
                if (i < SPU_ptr->byte_code_len) {
                    reg1 = SPU_ptr->byte_code[i];
                    fprintf_s(stderr, "Trying to PUSHR %hhX\n", reg1);
                    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, SPU_ptr->regs[reg1]);
                    break;
                }
                else {
                    CLEAR_RESOURCES();
                    return INCOMPLETE_ARGUMENT;
                }

            case POP_COMMAND:
                fprintf_s(stderr, "Trying to POP\n");
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, nullptr);
                break;

            case POPR_COMMAND:
                ++i;
                if (i < SPU_ptr->byte_code_len) {
                    reg1 = SPU_ptr->byte_code[i];
                    fprintf_s(stderr, "Trying to POPR %hhX\n", reg1);
                    CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &SPU_ptr->regs[reg1]);
                    break;
                }
                else {
                    CLEAR_RESOURCES();
                    return INCOMPLETE_ARGUMENT;
                }

            case ADD_COMMAND:
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op2);
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op1);
                fprintf_s(stderr, "Trying to ADD ");
                fprintf_s(stderr, stack_elem_frm, op1);
                fprintf_s(stderr, " ");
                fprintf_s(stderr, stack_elem_frm, op2);
                fprintf_s(stderr, "\n");
                CHECK_FUNC(My_stack_push, &SPU_ptr->stack, op1 + op2);
                break;

            case SUB_COMMAND:
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op2);
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op1);
                fprintf_s(stderr, "Trying to SUB ");
                fprintf_s(stderr, stack_elem_frm, op1);
                fprintf_s(stderr, " ");
                fprintf_s(stderr, stack_elem_frm, op2);
                fprintf_s(stderr, "\n");
                CHECK_FUNC(My_stack_push, &SPU_ptr->stack, op1 - op2);
                break;

            case MLT_COMMAND:
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op2);
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op1);
                fprintf_s(stderr, "Trying to MLT ");
                fprintf_s(stderr, stack_elem_frm, op1);
                fprintf_s(stderr, " ");
                fprintf_s(stderr, stack_elem_frm, op2);
                fprintf_s(stderr, "\n");
                CHECK_FUNC(My_stack_push, &SPU_ptr->stack, op1 * op2);
                break;

            case DIV_COMMAND:
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op2);
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op1);
                if (op2 == 0) { CLEAR_RESOURCES(); return EDOM; }
                fprintf_s(stderr, "Trying to DIV ");
                fprintf_s(stderr, stack_elem_frm, op1);
                fprintf_s(stderr, " ");
                fprintf_s(stderr, stack_elem_frm, op2);
                fprintf_s(stderr, "\n");
                CHECK_FUNC(My_stack_push, &SPU_ptr->stack, op1 / op2);
                break;

            case SQRT_COMMAND:
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op1);
                if (op1 < 0) { CLEAR_RESOURCES(); return EDOM; }
                fprintf_s(stderr, "Trying to SQRT ");
                fprintf_s(stderr, stack_elem_frm, op1);
                fprintf_s(stderr, "\n");
                CHECK_FUNC(My_stack_push, &SPU_ptr->stack, sqrt(op1));
                break;

            case POW_COMMAND:
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op2);
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op1);
                fprintf_s(stderr, "Trying to POW ");
                fprintf_s(stderr, stack_elem_frm, op1);
                fprintf_s(stderr, " ");
                fprintf_s(stderr, stack_elem_frm, op2);
                fprintf_s(stderr, "\n");
                CHECK_FUNC(My_stack_push, &SPU_ptr->stack, pow(op1, op2)); //TODO - how to check
                break;

            case IN_COMMAND:
                fprintf_s(stderr, "Trying to IN\n");
                if (scanf_s(stack_elem_frm, &op1) != 1) {
                    PRINT_LINE();
                    fprintf_s(stderr, "scanf_s failed\n");
                    CLEAR_RESOURCES();
                    return ferror(stdin);
                }

                CHECK_FUNC(My_stack_push, &SPU_ptr->stack, op1);
                break;

            case OUT_COMMAND:
                CHECK_FUNC(My_stack_pop, &SPU_ptr->stack, &op1);
                fprintf_s(stderr, "Trying to OUT");
                fprintf_s(stderr, stack_elem_frm, op1);
                fprintf_s(stderr, "\n");
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
