#include "Option_manager.h"
#include <string.h>

enum Option {
    HELP_OPTION,
    IN_OPTION,
    OUT_OPTION,
    __OPTIONS_COUNT,
};

static errno_t set_help_config(struct User_error *const error_ptr, struct Config *const config_ptr,
                               char const *const **const str_ptr,
                               char const *const *const end_str) {
    assert(error_ptr); assert(!error_ptr->is_valid);
    assert(config_ptr); assert(!config_ptr->is_valid);
    assert(str_ptr); assert(*str_ptr); assert(end_str);
    assert(*str_ptr != end_str); assert(!strcmp(**str_ptr, "--help"));

#undef FINAL_CODE
#define FINAL_CODE

    config_ptr->is_help = true;
    printf("Usage: SPU.exe [options] file...\nOptions:\n"
           "\t%-10s %s\n""\t%-10s %s\n""\t%-10s %s\n",
           "--help", "Display the information about options",
           "--in",   "Tells path to input-file by folowed parameter. If none is specified, Code.txt is used",
           "--out",  "Tells path to output-file by folowed parameter. If none is specified, Byte_code is used"
           );
    CLEAR_RESOURCES();
    return construct_User_error(error_ptr, NO_ERROR, 0);
}

static errno_t set_in_config(struct User_error *const error_ptr, struct Config *const config_ptr,
                             char const *const **const str_ptr,
                             char const *const *const end_str) {
    assert(error_ptr); assert(!error_ptr->is_valid);
    assert(config_ptr); assert(!config_ptr->is_valid);
    assert(str_ptr); assert(*str_ptr); assert(end_str);
    assert(*str_ptr != end_str); assert(!strcmp(**str_ptr, "--in"));

#undef FINAL_CODE
#define FINAL_CODE

    if (++*str_ptr == end_str) {
        CLEAR_RESOURCES();
        return construct_User_error(error_ptr, NOT_ENOUGH_OPTION_ARGUMENTS, 1, "--in");
    }

    CHECK_FUNC(fopen_s, &config_ptr->input_stream, **str_ptr, "r");
    CLEAR_RESOURCES();
    return construct_User_error(error_ptr, NO_ERROR, 0);
}

static errno_t set_out_config(struct User_error *const error_ptr, struct Config *const config_ptr,
                              char const *const **const str_ptr,
                              char const *const *const end_str) {
    assert(error_ptr); assert(!error_ptr->is_valid);
    assert(config_ptr); assert(!config_ptr->is_valid);
    assert(str_ptr); assert(*str_ptr); assert(end_str);
    assert(*str_ptr != end_str); assert(!strcmp(**str_ptr, "--out"));

#undef FINAL_CODE
#define FINAL_CODE

    if (++*str_ptr == end_str) {
        CLEAR_RESOURCES();
        return construct_User_error(error_ptr, NOT_ENOUGH_OPTION_ARGUMENTS, 1, "--out");
    }

    CHECK_FUNC(fopen_s, &config_ptr->output_stream, **str_ptr, "wb+");
    CLEAR_RESOURCES();
    return construct_User_error(error_ptr, NO_ERROR, 0);
}

static char const *const flag_option_arr[__OPTIONS_COUNT] = {
       "--help",
       "--in",
       "--out",
};

static errno_t (*const set_option_arr[__OPTIONS_COUNT])(struct User_error *const, struct Config *const,
                                                       char const *const **const,
                                                       char const *const *const) = {
       &set_help_config,
       &set_in_config,
       &set_out_config,
};

static errno_t select_option_setter(struct User_error *const error_ptr, struct Config *const config_ptr,
                                    char const *const **const str_ptr,
                                    char const *const *const end_str,
                                    bool *const used_options) {
    assert(error_ptr); assert(!error_ptr->is_valid);
    assert(config_ptr); assert(!config_ptr->is_valid);
    assert(str_ptr); assert(*str_ptr); assert(end_str);
    assert(*str_ptr != end_str);
    assert(used_options);

#undef FINAL_CODE
#define FINAL_CODE

    for (size_t i = 0; i < __OPTIONS_COUNT; ++i) {
        if (strcmp(**str_ptr, flag_option_arr[i])) { continue; }

        used_options[i] = true;
        CLEAR_RESOURCES();
        return set_option_arr[i](error_ptr, config_ptr, str_ptr, end_str);
    }

    CLEAR_RESOURCES();
    return construct_User_error(error_ptr, UNKNOWN_OPTION, 1, **str_ptr);
}

errno_t set_config(struct User_error *const error_ptr, struct Config *const config_ptr,
                   size_t const argc, char const *const *const argv) {
    assert(error_ptr); assert(!error_ptr->is_valid);
    assert(config_ptr); assert(!config_ptr->is_valid);
    assert(argc > 0); assert(argv); assert(*argv);

    char const *const *const end_str = argv + argc;
    bool used_options[__OPTIONS_COUNT] = {};

#undef FINAL_CODE
#define FINAL_CODE

    assert(end_str);

    for (char const *const *str = argv + 1; str != end_str; ++str) {
        assert(str); assert(*str);

        CHECK_FUNC(select_option_setter, error_ptr, config_ptr, &str, end_str, used_options);

        if (error_ptr->code != NO_ERROR) { CLEAR_RESOURCES(); return 0; }

        error_ptr->is_valid = false;
    }

    if (!used_options[IN_OPTION]) {
        CHECK_FUNC(fopen_s, &config_ptr->input_stream, "Code.txt", "r");
    }

    if (!used_options[OUT_OPTION]) {
        CHECK_FUNC(fopen_s, &config_ptr->output_stream, "Byte_code", "wb+");
    }

    ON_DEBUG(CHECK_FUNC(fopen_s, &config_ptr->text_output_stream, "Text_byte_code.txt", "w"));
    config_ptr->is_valid = true;
    CLEAR_RESOURCES();
    return construct_User_error(error_ptr, NO_ERROR, 0);
}
