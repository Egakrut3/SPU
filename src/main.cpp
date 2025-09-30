#include "Common.h"
#include "My_stack.h"
#include "Option_manager.h"
#include "User_error_handler.h"

int main(int const argc, char const *const *const argv) {
    assert(argc > 0); assert(argv); assert(*argv);

    struct Config cur_config = {};
    struct User_error cur_error = {};

#undef FINAL_CODE
#define FINAL_CODE                      \
    destruct_Config(&cur_config);       \
    destruct_User_error(&cur_error);

    MAIN_CHECK_FUNC(set_config, &cur_error, &cur_config, argc, argv);
    if (handle_User_error(&cur_error)) { CLEAR_RESOURCES(); return 0; }
    if (cur_config.is_help) { CLEAR_RESOURCES(); return 0; }

    char str[20] = {};
    while(fscanf("%s", &str))
    {
        fprintf(stderr, "%s\n", str);
    }

    colored_printf(GREEN, BLACK, "\n\n\nCOMMIT GITHUB\n\n");
    CLEAR_RESOURCES();
    return 0;
}
