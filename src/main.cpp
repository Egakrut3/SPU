#include "Common.h"
#include "Option_manager.h"
#include "User_error_handler.h"
#include "Assembler.h"
#include "SPU.h"

int main(int const argc, char const *const *const argv) {
    assert(argc > 0); assert(argv); assert(*argv);

    struct Config cur_config    = {};
    struct User_error cur_error = {};
#undef FINAL_CODE
#define FINAL_CODE                      \
    destruct_Config(&cur_config);       \
    destruct_User_error(&cur_error);

    MAIN_CHECK_FUNC(set_config, &cur_error, &cur_config, argc, argv);
    if (handle_User_error(&cur_error)) { CLEAR_RESOURCES(); return 0; }
    if (cur_config.is_help)            { CLEAR_RESOURCES(); return 0; }

    destruct_User_error(&cur_error);
    MAIN_CHECK_FUNC(make_byte_code, &cur_error, cur_config.input_stream, cur_config.output_stream
                                    ON_DEBUG(, cur_config.text_output_stream));
    if (handle_User_error(&cur_error)) { CLEAR_RESOURCES(); return 0; }

    rewind(cur_config.output_stream);
    SPU_CREATE(cur_SPU, 5, cur_config.output_stream, MAIN_CHECK_FUNC);
    SPU_DUMP(stderr, cur_SPU, ~0);

    colored_printf(GREEN, BLACK, "\n\n\nCOMMIT GITHUB\n\n");
    CLEAR_RESOURCES();
    return 0;
}
