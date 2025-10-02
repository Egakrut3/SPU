#include "Assembler.h"
#include <string.h>

errno_t make_byte_code(User_error *const error_ptr,
                       FILE *const code_stream, FILE *const byte_code_stream) {
    assert(code_stream);

    size_t cur_len = 0;
    char *byte_code = (char *)calloc(MAX_BYTE_CODE_LEN, sizeof(char));
#undef FINAL_CODE
#define FINAL_CODE

    char str[5] = {};
    while (cur_len < MAX_BYTE_CODE_LEN) {
        if (fscanf_s(code_stream, "%s", str, 5) != 1) {
            PRINT_LINE();
            fprintf_s(stderr, "fscanf_s failed");
            CLEAR_RESOURCES();
            return ferror(code_stream);
        }

        if (!strcmp(str, "HLT")) {
            cur_len += sprintf_s(byte_code + cur_len, MAX_BYTE_CODE_LEN - cur_len, "%hhu", 0);
            fprintf_s(byte_code_stream, "%s", byte_code);
            return construct_User_error(error_ptr, NO_ERROR, 0);
        }

        if (!strcmp(str, "PUSH")) {
            cur_len += sprintf_s(byte_code + cur_len, MAX_BYTE_CODE_LEN - cur_len, "%hhu ", 1);
            assembler_elem a = 0;
            if (fscanf_s(code_stream, "%LG", &a) != 1) {
                PRINT_LINE();
                fprintf_s(stderr, "fscanf_s failed");
                CLEAR_RESOURCES();
                return ferror(code_stream);
            }

            cur_len += sprintf_s(byte_code + cur_len, MAX_BYTE_CODE_LEN - cur_len, "%LG\n", a);
            continue;
        }

        if (!strcmp(str, "ADD")) {
            cur_len += sprintf_s(byte_code + cur_len, MAX_BYTE_CODE_LEN - cur_len, "%hhu\n", 2);
            continue;
        }

        if (!strcmp(str, "SUB")) {
            cur_len += sprintf_s(byte_code + cur_len, MAX_BYTE_CODE_LEN - cur_len, "%hhu\n", 3);
            continue;
        }

        if (!strcmp(str, "MLT")) {
            cur_len += sprintf_s(byte_code + cur_len, MAX_BYTE_CODE_LEN - cur_len, "%hhu\n", 4);
            continue;
        }

        if (!strcmp(str, "DIV")) {
            cur_len += sprintf_s(byte_code + cur_len, MAX_BYTE_CODE_LEN - cur_len, "%hhu\n", 5);
            continue;
        }

        if (!strcmp(str, "SQRT")) {
            cur_len += sprintf_s(byte_code + cur_len, MAX_BYTE_CODE_LEN - cur_len, "%hhu\n", 6);
            continue;
        }

        if (!strcmp(str, "POW")) {
            cur_len += sprintf_s(byte_code + cur_len, MAX_BYTE_CODE_LEN - cur_len, "%hhu\n", 7);
            continue;
        }

        if (!strcmp(str, "OUT")) {
            cur_len += sprintf_s(byte_code + cur_len, MAX_BYTE_CODE_LEN - cur_len, "%hhu\n", 8);
            continue;
        }

        return construct_User_error(error_ptr, UNKNOWN_OPERATION, 1, str);
    }

    return construct_User_error(error_ptr, CODE_TOO_LONG, 0);
}
