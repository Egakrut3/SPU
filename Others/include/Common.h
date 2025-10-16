#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include "Colored_printf.h"
#include <string.h>

typedef unsigned char byte_elem_t;

#define PRINT_LINE() colored_error_printf(RED, BLACK,                                                   \
                                          "Error found, file " __FILE__ ", line %d in \"%s\" function\n",   \
                                          __LINE__, __func__)

#define CLEAR_RESOURCES()   \
do {                        \
    FINAL_CODE              \
} while (false)

#define CHECK_FUNC(func, ...)                       \
do {                                                \
    errno_t err_val = func(__VA_ARGS__);            \
    if (err_val) {                                  \
        PRINT_LINE();                               \
        fprintf_s(stderr, "Code %d: ", err_val);    \
        perror(#func " failed");                    \
        CLEAR_RESOURCES();                          \
        return errno;                               \
    }                                               \
} while (false)

#define MAIN_CHECK_FUNC(func, ...)                  \
do {                                                \
    errno_t err_val = func(__VA_ARGS__);            \
    if (err_val) {                                  \
        PRINT_LINE();                               \
        fprintf_s(stderr, "Code %d: ", err_val);    \
        perror(#func " failed");                    \
        CLEAR_RESOURCES();                          \
        return 0;                                   \
    }                                               \
} while (false)

#ifdef _DEBUG
#define ON_DEBUG(...) __VA_ARGS__
#else
#define ON_DEBUG(...)
#endif

struct Position_info {
    char const *file_name,
               *function_name;
    size_t     line;
};

struct Var_info {
    Position_info position;
    char const    *name;
};

size_t const CANARY = 0XFACE'FACE'FACE'FACE;

errno_t My_calloc(void **dest, size_t num, size_t size);

errno_t My_fread(void *__restrict buffer, size_t size, size_t num,
                 FILE *__restrict stream);

errno_t My_fwrite(const void *__restrict buffer, size_t size, size_t num,
                  FILE *__restrict stream);

errno_t get_path_filesize(char const *path, __int64 *filesize);

errno_t get_opened_filesize(FILE *cur_file, __int64 *filesize);

errno_t get_all_content(FILE *stream, size_t *filesize_dest, char **buffer);

errno_t My_sscanf_s(size_t count, char const *__restrict buffer,
                    char const *__restrict format, ...);

#endif
