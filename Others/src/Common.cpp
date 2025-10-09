#include "Common.h"
#include <stdarg.h>

errno_t My_calloc(void **const dest, size_t const num, size_t const size) {
    assert(dest);

    #undef FINAL_CODE
    #define FINAL_CODE

    if (num == 0) {
        *dest = nullptr;
        CLEAR_RESOURCES();
        return 0;
    }

    void *ptr = calloc(num, size);
    if (!ptr) {
        CLEAR_RESOURCES();
        return errno;
    }

    *dest = ptr;
    CLEAR_RESOURCES();
    return 0;
}

errno_t My_fscanf_s(size_t const count, FILE *__restrict const stream,
                    char const *__restrict const format, ...) {
    assert(stream); assert(format);

    va_list args = nullptr;
    va_start(args, format);
    #undef FINAL_CODE
    #define FINAL_CODE  \
    va_end(args);

    if (vfscanf_s(stream, format, args) != count) {
        CLEAR_RESOURCES();
        return ferror(stream);
    }

    CLEAR_RESOURCES();
    return 0;
}

errno_t My_fwrite(const void *__restrict const buffer, size_t const size, size_t const num,
                  FILE *__restrict const stream) {
    assert(buffer); assert(stream);

    #undef FINAL_CODE
    #define FINAL_CODE

    if (fwrite(buffer, size, num, stream) != num) {
        CLEAR_RESOURCES();
        return ferror(stream);
    }

    CLEAR_RESOURCES();
    return 0;
}

static size_t const UNALIGN8_MASK = 0B111,
                    ALIGN8_MASK   = ~UNALIGN8_MASK;

size_t get_assembler_aligned(size_t const x) {
    return (x + UNALIGN8_MASK) & ALIGN8_MASK;
}
