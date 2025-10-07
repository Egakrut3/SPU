#include "SPU.h"
#include "My_stack.h"
#include <math.h>

errno_t execute_byte_code(User_error *const error_ptr, FILE *const byte_code_stream) {
    assert(error_ptr); assert(byte_code_stream);

#undef FINAL_CODE
#define FINAL_CODE

    STACK_CREATE(stk, 1, CHECK_FUNC);
#undef FINAL_CODE
#define FINAL_CODE          \
    My_stack_Dtor(&stk);

    byte_elem_t cur_command = -1;
    while (true) {
        if (fscanf(byte_code_stream, "%hhu", &cur_command) != 1) {
            PRINT_LINE();
            fprintf(stderr, "fscanf failed");
            CLEAR_RESOURCES();
            return ferror(byte_code_stream);
        }

        assembler_elem_t a = 0,
                         b = 0;
        switch (cur_command) {
            case 0:
                CLEAR_RESOURCES();
                return construct_User_error(error_ptr, NO_ERROR, 0);

            case 1:
                if (fscanf_s(byte_code_stream, "%lG", &a) != 1) {
                    PRINT_LINE();
                    fprintf_s(stderr, "fscanf_s failed");
                    CLEAR_RESOURCES();
                    return ferror(byte_code_stream);
                }

                CHECK_FUNC(My_stack_push, &stk, a);
                break;

            case 2:
                CHECK_FUNC(My_stack_pop, &stk, &a);
                CHECK_FUNC(My_stack_pop, &stk, &b);
                CHECK_FUNC(My_stack_push, &stk, b + a);
                break;

            case 3:
                CHECK_FUNC(My_stack_pop, &stk, &a);
                CHECK_FUNC(My_stack_pop, &stk, &b);
                CHECK_FUNC(My_stack_push, &stk, b - a);
                break;

            case 4:
                CHECK_FUNC(My_stack_pop, &stk, &a);
                CHECK_FUNC(My_stack_pop, &stk, &b);
                CHECK_FUNC(My_stack_push, &stk, b * a);
                break;

            case 5:
                CHECK_FUNC(My_stack_pop, &stk, &a);
                CHECK_FUNC(My_stack_pop, &stk, &b);
                CHECK_FUNC(My_stack_push, &stk, b - a);
                break;

            case 6:
                CHECK_FUNC(My_stack_pop, &stk, &a);
                CHECK_FUNC(My_stack_push, &stk, sqrt(a));
                break;

            case 7:
                CHECK_FUNC(My_stack_pop, &stk, &a);
                CHECK_FUNC(My_stack_pop, &stk, &b);
                CHECK_FUNC(My_stack_push, &stk, pow(b, a));
                break;

            case 8:
                CHECK_FUNC(My_stack_pop, &stk, &a);
                printf_s("%LG\n", a);
                break;

            default:
                CLEAR_RESOURCES();
                char str_cur_command[5] = {};
                sprintf_s(str_cur_command, 5, "%hhu", cur_command);
                return construct_User_error(error_ptr, UNKNOWN_OPERATION, 1, str_cur_command);
        }
    }
}
