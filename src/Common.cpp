#include "Common.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

errno_t construct_User_error(struct User_error *const error_ptr, enum User_error_code const code,
                                                                 size_t const str_cnt, ...) {
    assert(error_ptr); assert(!error_ptr->is_valid);

    va_list arg_list = nullptr;
    va_start(arg_list, str_cnt);

#undef FINAL_CODE
#define FINAL_CODE      \
    va_end(arg_list);

    error_ptr->code    = code;
    error_ptr->str_cnt = str_cnt;
    if (!str_cnt) {
        error_ptr->data     = nullptr;
        error_ptr->is_valid = true;
        CLEAR_RESOURCES();
        return 0;
    }

    error_ptr->data = (char **)calloc(str_cnt, sizeof(char *));
    if (!error_ptr->data) {
        PRINT_LINE();
        perror("calloc failed");
        CLEAR_RESOURCES();
        return errno;
    }

    for (size_t i = 0; i < str_cnt; ++i) {
        char const *const new_str = va_arg(arg_list, char const *);

        assert(new_str);

        error_ptr->data[i] = strdup(new_str);
        if (!error_ptr->data[i]) {
            PRINT_LINE();
            perror("strdup failed");
            CLEAR_RESOURCES();
            return errno;
        }
    }

    error_ptr->is_valid = true;
    CLEAR_RESOURCES();
    return 0;
}

void destruct_User_error(struct User_error *const error_ptr) {
    assert(error_ptr);

#undef FINAL_CODE
#define FINAL_CODE

    error_ptr->is_valid = false;
    for (size_t i = 0; i < error_ptr->str_cnt; ++i) {
        assert(error_ptr->data); assert(error_ptr->data[i]);

        free(error_ptr->data[i]);
    }

    free(error_ptr->data);
    CLEAR_RESOURCES();
}

void destruct_Config(struct Config *const config_ptr) {
    assert(config_ptr);

#undef FINAL_CODE
#define FINAL_CODE

    config_ptr->is_valid = false;
    fclose(config_ptr->input_stream);
    fclose(config_ptr->output_stream);
    ON_DEBUG(fclose(config_ptr->text_output_stream));
    CLEAR_RESOURCES();
}
