#include "Common.h"
#include "Assembler.h"
#include "Disassembler.h"
#include "SPU.h"
//TODO - standartize #undef FINAL_CODE
//TODO - modify Makefile
//TODO - how to add Common to both projects

int main(int const argc, char const *const *const argv) {
    assert(argc > 0); assert(argv); assert(*argv);

    FILE *code_stream      = nullptr,
         *byte_code_stream = nullptr;
    ON_DEBUG(
    FILE *text_byte_code_stream = nullptr,
         *dis_code_stream       = nullptr;
    )
    //TODO - don't work
    #undef FINAL_CODE
    #define FINAL_CODE                  \
        fclose(code_stream);            \
        fclose(byte_code_stream);       \
        ON_DEBUG(                       \
        fclose(text_byte_code_stream);  \
        fclose(dis_code_stream);        \
        )

    MAIN_CHECK_FUNC(fopen_s, &code_stream,           "Code.txt",           "r");
    MAIN_CHECK_FUNC(fopen_s, &byte_code_stream,      "Byte_code",          "wb+");
    ON_DEBUG(
    MAIN_CHECK_FUNC(fopen_s, &text_byte_code_stream, "Text_byte_code.txt", "w");
    MAIN_CHECK_FUNC(fopen_s, &dis_code_stream,       "Dis_code.txt",       "w");
    )

    MAIN_CHECK_FUNC(compilate, code_stream, byte_code_stream
                                 ON_DEBUG(, text_byte_code_stream));

    /*rewind(byte_code_stream);
    MAIN_CHECK_FUNC(disassembly_byte_code, byte_code_stream, dis_code_stream);*/

    rewind(byte_code_stream);
    SPU_CREATE(cur_SPU, 5, byte_code_stream, MAIN_CHECK_FUNC);
    MAIN_CHECK_FUNC(SPU_execute, &cur_SPU);
    //SPU_DUMP(stderr, &cur_SPU, 0);

    colored_printf(GREEN, BLACK, "\n\n\nCOMMIT GITHUB\n\n");
    CLEAR_RESOURCES();
    return 0;
}
