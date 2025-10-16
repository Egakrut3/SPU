#include "Assembler.h"

static size_t const ASM_TOKEN_MAX_LEN = 5;

#undef FINAL_CODE
#define FINAL_CODE

errno_t Assembler_Ctor(Assembler *const asm_ptr, FILE *const code_stream) {
    assert(asm_ptr); assert(!asm_ptr->is_valid); assert(code_stream);

    for (size_t i = 0; i < LABELS_COUNT; ++i) {
        asm_ptr->labels[i] = -1;
    }

    CHECK_FUNC(get_all_content, code_stream, &asm_ptr->code_len, &asm_ptr->code);
    asm_ptr->byte_code_len = 0;
    CHECK_FUNC(My_calloc, (void **)&asm_ptr->byte_code, BYTE_CODE_MAX_LEN, sizeof(*asm_ptr->byte_code));
    ON_DEBUG(
    asm_ptr->text_byte_code_len = 0;
    CHECK_FUNC(My_calloc, (void **)&asm_ptr->text_byte_code, BYTE_CODE_MAX_LEN + 1,
                          sizeof(*asm_ptr->text_byte_code));
    )

    asm_ptr->is_valid = true;
    return 0;
}

void Assembler_Dtor(Assembler *const asm_ptr) {
    assert(asm_ptr);

    free(asm_ptr->byte_code);
    ON_DEBUG(free(asm_ptr->text_byte_code);)
}
