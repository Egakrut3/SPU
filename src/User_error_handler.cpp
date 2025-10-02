#include "User_error_handler.h"

bool handle_User_error(struct User_error const *const error_ptr) {
    assert(error_ptr); assert(error_ptr->is_valid);

#undef FINAL_CODE
#define FINAL_CODE

    switch (error_ptr->code) {
        case NO_ERROR:
            assert(error_ptr->str_cnt == 0); assert(!error_ptr->data);

            CLEAR_RESOURCES();
            return false;

        case UNKNOWN_OPTION:
            assert(error_ptr->str_cnt == 1); assert(error_ptr->data); assert(error_ptr->data[0]);

            colored_error_printf(RED, BLACK, "\"%s\" option is unknown\n", error_ptr->data[0]);
            CLEAR_RESOURCES();
            return true;

        case NOT_ENOUGH_OPTION_ARGUMENTS:
            assert(error_ptr->str_cnt == 1); assert(error_ptr->data); assert(error_ptr->data[0]);

            colored_error_printf(RED, BLACK, "\"%s\" option requires more arguments after itself\n",
                                 error_ptr->data[0]);
            CLEAR_RESOURCES();
            return true;

        case UNKNOWN_OPERATION:
            assert(error_ptr->str_cnt == 1); assert(error_ptr->data); assert(error_ptr->data[0]);

            colored_error_printf(RED, BLACK, "\"%s\" operation is unknown\n", error_ptr->data[0]);
            CLEAR_RESOURCES();
            return true;

        case CODE_TOO_LONG:
            assert(error_ptr->str_cnt == 0); assert(!error_ptr->data);

            colored_error_printf(RED, BLACK,
                                 "Assembler code is too long or doesn't contain \"HLT\" operation\n");
            CLEAR_RESOURCES();
            return true;

        case __INVALID_ERROR:
        default:
            PRINT_LINE();
            abort();
            CLEAR_RESOURCES(); //TODO - is it necessary?
            return true;
    }
}
