#include "Assembler.h"
#include <string.h>

static size_t const BYTE_CODE_MAX_LEN   = 0X1000,
                    ASM_COMMAND_MAX_LEN = 5;

static size_t const UNALIGN8_MASK = 0B111,
                    ALIGN8_MASK   = ~UNALIGN8_MASK;

static size_t get_assembler_aligned(size_t const x) {
    return (x + UNALIGN8_MASK) & ALIGN8_MASK;
}

errno_t make_byte_code(User_error *const error_ptr,
                       FILE *const code_stream, FILE *const byte_code_stream
                       ON_DEBUG(, FILE *const text_byte_code_stream)) {
    assert(error_ptr); assert(!error_ptr->is_valid);
    assert(code_stream); assert(byte_code_stream);

    size_t     cur_len      = 0
    ON_DEBUG(, text_cur_len = 0);
    byte_elem_t    *byte_code      = (byte_elem_t *)calloc(BYTE_CODE_MAX_LEN, sizeof(byte_elem_t));
    ON_DEBUG(char  *text_byte_code = (char *)calloc(BYTE_CODE_MAX_LEN + 1, sizeof(char));)
#undef FINAL_CODE
#define FINAL_CODE                      \
    free(byte_code);                    \
    ON_DEBUG(free(text_byte_code);)

    char cur_command[ASM_COMMAND_MAX_LEN + 1] = {};
    while (cur_len      < BYTE_CODE_MAX_LEN ON_DEBUG(and
           text_cur_len < BYTE_CODE_MAX_LEN)) {
        if (fscanf_s(code_stream, "%s", cur_command, ASM_COMMAND_MAX_LEN + 1) != 1) {
            PRINT_LINE();
            fprintf_s(stderr, "fscanf_s failed");
            CLEAR_RESOURCES();
            return ferror(code_stream);
        }

        if (!strcmp(cur_command, "HLT")) {
            byte_code[cur_len++] = HLT_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + cur_len, BYTE_CODE_MAX_LEN - cur_len,
                                               "%hhX", HLT_COMMAND);)

            fwrite(byte_code, sizeof(byte_elem_t), cur_len, byte_code_stream); //TODO - possible write size
            ON_DEBUG(fprintf_s(text_byte_code_stream, "%s", text_byte_code));
            CLEAR_RESOURCES();
            return construct_User_error(error_ptr, NO_ERROR, 0);
        }

        if (!strcmp(cur_command, "PUSH")) {
            byte_code[cur_len++] = PUSH_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + cur_len, BYTE_CODE_MAX_LEN - cur_len,
                                               "%hhX ", PUSH_COMMAND);)

            assembler_elem_t a = 0;
            if (fscanf_s(code_stream, assembler_elem_frm, &a) != 1) {
                PRINT_LINE();
                fprintf_s(stderr, "fscanf_s failed");
                CLEAR_RESOURCES();
                return ferror(code_stream);
            }

            cur_len = get_assembler_aligned(cur_len);
            if (cur_len == BYTE_CODE_MAX_LEN) {
                CLEAR_RESOURCES();
                return construct_User_error(error_ptr, BYTE_CODE_TOO_LONG, 0);
            }
            *(assembler_elem_t *)(byte_code + cur_len) = a;
            cur_len += sizeof(assembler_elem_t);
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + cur_len, BYTE_CODE_MAX_LEN - cur_len,
                                               assembler_elem_frm, a);)
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + cur_len, BYTE_CODE_MAX_LEN - cur_len,
                                               "\n");)
            continue;
        }

        if (!strcmp(cur_command, "ADD")) {
            byte_code[cur_len++] = ADD_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + cur_len, BYTE_CODE_MAX_LEN - cur_len,
                                               "%hhX ", ADD_COMMAND);)
            continue;
        }

        if (!strcmp(cur_command, "SUB")) {
            byte_code[cur_len++] = SUB_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + cur_len, BYTE_CODE_MAX_LEN - cur_len,
                                               "%hhX ", SUB_COMMAND);)
            continue;
        }

        if (!strcmp(cur_command, "MLT")) {
            byte_code[cur_len++] = MLT_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + cur_len, BYTE_CODE_MAX_LEN - cur_len,
                                               "%hhX ", MLT_COMMAND);)
            continue;
        }

        if (!strcmp(cur_command, "DIV")) {
            byte_code[cur_len++] = DIV_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + cur_len, BYTE_CODE_MAX_LEN - cur_len,
                                               "%hhX ", DIV_COMMAND);)
            continue;
        }

        if (!strcmp(cur_command, "SQRT")) {
            byte_code[cur_len++] = SQRT_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + cur_len, BYTE_CODE_MAX_LEN - cur_len,
                                               "%hhX ", SQRT_COMMAND);)
            continue;
        }

        if (!strcmp(cur_command, "POW")) {
            byte_code[cur_len++] = POW_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + cur_len, BYTE_CODE_MAX_LEN - cur_len,
                                               "%hhX ", POW_COMMAND);)
            continue;
        }

        if (!strcmp(cur_command, "OUT")) {
            byte_code[cur_len++] = OUT_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + cur_len, BYTE_CODE_MAX_LEN - cur_len,
                                               "%hhX ", OUT_COMMAND);)
            continue;
        }

        CLEAR_RESOURCES();
        return construct_User_error(error_ptr, UNKNOWN_OPERATION, 1, cur_command);
    }

    CLEAR_RESOURCES();
    return construct_User_error(error_ptr, BYTE_CODE_TOO_LONG, 0);
}
