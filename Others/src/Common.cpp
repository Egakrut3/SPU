#include "Common.h"
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

#undef FINAL_CODE
#define FINAL_CODE

errno_t My_calloc(void **const dest, size_t const num, size_t const size) {
    assert(dest);

    if (num == 0) {
        *dest = nullptr;
        return 0;
    }

    void *ptr = calloc(num, size);
    if (!ptr) {
        return errno;
    }

    *dest = ptr;
    return 0;
}

errno_t My_fread(void *__restrict const buffer, size_t const size, size_t const num,
                 FILE *__restrict const stream) {
    assert(buffer); assert(stream);

    if (fread(buffer, size, num, stream) != num) {
        return ferror(stream);
    }

    return 0;
}

errno_t My_fwrite(const void *__restrict const buffer, size_t const size, size_t const num,
                  FILE *__restrict const stream) {
    assert(buffer); assert(stream);

    fprintf_s(stderr, "Hello3\n");

    if (fwrite(buffer, size, num, stream) != num) {
        fprintf_s(stderr, "Hello4\n");
        return ferror(stream);
    }

    fprintf_s(stderr, "Hello5\n");

    return 0;
}

errno_t get_path_filesize(char const *const path, __int64 *const filesize)
{
    assert(path); assert(filesize);

    struct _stat64 buffer = {};

    CHECK_FUNC(_stat64, path, &buffer);
    *filesize = buffer.st_size;

    assert(*filesize > 0);

    return 0;
}

errno_t get_opened_filesize(FILE *const cur_file, __int64 *const filesize)
{
    assert(cur_file); assert(filesize);

    struct _stat64 buffer = {};

    CHECK_FUNC(_fstat64, fileno(cur_file), &buffer);
    *filesize = buffer.st_size;

    assert(*filesize > 0);

    return 0;
}

errno_t get_all_content(FILE *const stream, size_t *const filesize_dest, char **const buffer)
{
    assert(stream); assert(buffer);

    size_t filesize = 0;
    get_opened_filesize(stream, (__int64 *)&filesize);
    if (filesize_dest) {
        *filesize_dest = filesize;
    }

    CHECK_FUNC(My_calloc, (void **)buffer, filesize + 1, sizeof(char));
    CHECK_FUNC(My_fread, *buffer, sizeof(char), filesize, stream);

    return 0;
}

errno_t My_sscanf_s(size_t const count, char const *__restrict const buffer,
                    char const *__restrict const format, ...) {
    assert(format);

    va_list args = nullptr;
    va_start(args, format);
    #undef FINAL_CODE
    #define FINAL_CODE  \
        va_end(args);

    if (vsscanf_s(buffer, format, args) != (ssize_t)count) {
        CLEAR_RESOURCES();
        return ferror(stdin);
    }

    CLEAR_RESOURCES();
    return 0;
}
