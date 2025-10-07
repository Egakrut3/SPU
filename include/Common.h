#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include "Colored_printf.h"

typedef unsigned char byte_elem_t;

#define PRINT_LINE() colored_error_printf(RED, BLACK,                                               \
                                          "Error found, file " __FILE__ ", line %d\n", __LINE__)

#define CLEAR_RESOURCES()   \
do {                        \
    FINAL_CODE              \
} while (false)

#define CHECK_FUNC(func, ...)       \
do {                                \
    if (func(__VA_ARGS__)) {        \
        PRINT_LINE();               \
        perror(#func " failed");    \
        CLEAR_RESOURCES();          \
        return errno;               \
    }                               \
} while (false)

#define MAIN_CHECK_FUNC(func, ...)  \
do {                                \
    if (func(__VA_ARGS__)) {        \
        PRINT_LINE();               \
        perror(#func " failed");    \
        CLEAR_RESOURCES();          \
        return 0;                   \
    }                               \
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

enum User_error_code {
    NO_ERROR,                    ///< Code 0
    UNKNOWN_OPTION,              ///< Code 1
    NOT_ENOUGH_OPTION_ARGUMENTS, ///< Code 2
    UNKNOWN_OPERATION,           ///< Code 3
    BYTE_CODE_TOO_LONG,               ///< Code 4
    __INVALID_ERROR,             ///< Code 5
};

struct User_error {
    char                   **data;
    size_t                 str_cnt;
    enum User_error_code   code;

    bool                   is_valid;
};

errno_t construct_User_error(struct User_error *error_ptr, enum User_error_code code,
                                                           size_t str_cnt, ...);

void destruct_User_error(struct User_error *error_ptr);

struct Config {
    FILE       *input_stream,
               *output_stream
    ON_DEBUG(, *text_output_stream);
    bool       is_help;

    bool       is_valid;
};

void destruct_Config(struct Config *config_ptr);

enum Asm_command_code : byte_elem_t {
    HLT_COMMAND,
    PUSH_COMMAND,
    ADD_COMMAND,
    SUB_COMMAND,
    MLT_COMMAND,
    DIV_COMMAND,
    SQRT_COMMAND,
    POW_COMMAND,
    OUT_COMMAND,
};

#endif
