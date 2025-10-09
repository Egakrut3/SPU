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

#define PRINT_LINE() colored_error_printf(RED, BLACK,                                                   \
                                          "Error found, file " __FILE__ ", line %d in \"%s\" function\n",   \
                                          __LINE__, __func__)

//TODO - possible make macros for return
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

#define MAIN_CHECK_FUNC(func, ...)          \
do {                                        \
    errno_t err_val = func(__VA_ARGS__);    \
    if (err_val) {                          \
        PRINT_LINE();                       \
        printf("Code %d: ", err_val);       \
        perror(#func " failed");            \
        CLEAR_RESOURCES();                  \
        return 0;                           \
    }                                       \
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

errno_t My_fscanf_s(size_t count, FILE *__restrict stream,
                    char const *__restrict format, ...);

errno_t My_fwrite(const void *__restrict buffer, size_t size, size_t num,
                  FILE *__restrict stream);

enum Asm_command_code : byte_elem_t {
    HLT_COMMAND,
    PUSH_COMMAND,
    PUSHR_COMMAND,
    POP_COMMAND,
    POPR_COMMAND,
    ADD_COMMAND,
    SUB_COMMAND,
    MLT_COMMAND,
    DIV_COMMAND,
    SQRT_COMMAND,
    POW_COMMAND,
    IN_COMMAND,
    OUT_COMMAND,
    JMP_COMMAND,
    JB_COMMAND,
    JBE_COMMAND,
    JA_COMMAND,
    JAE_COMMAND,
    JE_COMMAND,
    JNE_COMMAND,
    __ASM_COMMAND_COUNT,
};

size_t get_assembler_aligned(size_t x);

#endif
