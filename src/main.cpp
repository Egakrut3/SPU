#include "Common.h"
#include "Option_manager.h"
#include "Assembler.h"
#include "SPU.h"
//TODO - possible separate command functions from .cpp files
//TODO - review code
//TODO - review Makefile

int main(int const argc, char const *const *const argv) {
    assert(argc > 0); assert(argv); assert(*argv);

    Config cur_config = {};
    FILE *byte_code_stream = nullptr;
    ON_DEBUG(
    FILE *text_byte_code_stream = nullptr;
    )
    //TODO - don't work
    #undef FINAL_CODE
    #define FINAL_CODE                  \
        Config_Dtor(&cur_config);       \
        fclose(byte_code_stream);       \
        ON_DEBUG(                       \
        fclose(text_byte_code_stream);  \
        )

    MAIN_CHECK_FUNC(Config_Ctor, &cur_config, argc, argv);
    MAIN_CHECK_FUNC(fopen_s, &byte_code_stream,      "Byte_code",          "wb+");
    ON_DEBUG(
    MAIN_CHECK_FUNC(fopen_s, &text_byte_code_stream, "Text_byte_code.txt", "w");
    )

    MAIN_CHECK_FUNC(compilate, cur_config.input_stream, byte_code_stream
                                             ON_DEBUG(, text_byte_code_stream));

    rewind(byte_code_stream);
    SPU_CREATE(cur_SPU, 5, byte_code_stream, MAIN_CHECK_FUNC);
    MAIN_CHECK_FUNC(SPU_execute, &cur_SPU);
    //SPU_DUMP(stderr, cur_SPU, 0, MAIN_CHECK_FUNC);

    colored_printf(GREEN, BLACK, "\n\n\nCOMMIT GITHUB\n\n");
    CLEAR_RESOURCES();
    return 0;
}
