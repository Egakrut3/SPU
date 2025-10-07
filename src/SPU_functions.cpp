#include "SPU.h"
#include <math.h>

errno_t SPU_execute(SPU *const SPU_ptr) {
    assert(SPU_ptr);

    stack_elem_t op1 = {},
                 op2 = {};
    byte_elem_t reg1 = {};
#undef FINAL_CODE
#define FINAL_CODE

    ON_DEBUG(CHECK_FUNC(SPU_verify, SPU_ptr));

    for (size_t IC = 0; IC < SPU_ptr->byte_code_len; ++IC) {
        //fprintf_s(stderr, "IC = %zu\n", IC);
        switch (SPU_ptr->byte_code[IC]) {
            case HLT_COMMAND:
                CLEAR_RESOURCES();
                fprintf_s(stderr, "Finishing execution\n");
                if (SPU_ptr->stack.size) { return STACK_NOT_EMPTY_AFTER_EXECUTION; }
                else { return 0; }

            case PUSH_COMMAND:
                IC = get_assembler_aligned(IC + 1);
                if (IC < SPU_ptr->byte_code_len) {
                    op1 = *(stack_elem_t *)(SPU_ptr->byte_code + IC);
                    fprintf_s(stderr, "Trying to PUSH ");
                    fprintf_s(stderr, stack_elem_frm, op1);
                    fprintf_s(stderr, "\n");
                    CHECK_FUNC(My_stack_push, &SPU_ptr->stack, op1);
                    IC += sizeof(stack_elem_t) - 1;
                    break;
                }
                else {
                    CLEAR_RESOURCES();
                    return INCOMPLETE_ARGUMENT;
                }

            case PUSHR_COMMAND:
                ++IC;
                if (IC < SPU_ptr->byte_code_len) {
                    reg1 = SPU_ptr->byte_code[IC];
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
                ++IC;
                if (IC < SPU_ptr->byte_code_len) {
                    reg1 = SPU_ptr->byte_code[IC];
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
                fprintf_s(stderr, "Trying to OUT ");
                fprintf_s(stderr, stack_elem_frm, op1);
                fprintf_s(stderr, "\n");
                printf_s(stack_elem_frm, op1);
                printf_s("\n");
                getchar();
                break;

            case JMP_COMMAND:
                fprintf_s(stderr, "Trying to JMP\n");
                IC = SPU_ptr->byte_code[IC + 1] - 1;
                break;

            default:
                CLEAR_RESOURCES();
                return UNKNOWN_COMMAND;
        }
    }

    CLEAR_RESOURCES();
    return NO_HLT_COMMAND;
}
