#include "Assembler.h"
#include <string.h>

static size_t const BYTE_CODE_MAX_LEN   = 0X1000,
                    ASM_COMMAND_MAX_LEN = 5;

static size_t const UNALIGN8_MASK = 0B111,
                    ALIGN8_MASK   = ~UNALIGN8_MASK;

size_t get_assembler_aligned(size_t const x) {
    return (x + UNALIGN8_MASK) & ALIGN8_MASK;
}

errno_t make_byte_code(FILE *const code_stream, FILE *const byte_code_stream
                       ON_DEBUG(, FILE *const text_byte_code_stream)) {
    assert(code_stream); assert(byte_code_stream);

    size_t     cur_len      = 0
    ON_DEBUG(, text_cur_len = 0);
    byte_elem_t    *const byte_code      = (byte_elem_t *)calloc(BYTE_CODE_MAX_LEN, sizeof(byte_elem_t));
    ON_DEBUG(char  *const text_byte_code = (char *)calloc(BYTE_CODE_MAX_LEN + 1, sizeof(char));)
#undef FINAL_CODE
#define FINAL_CODE                      \
    free(byte_code);                    \
    ON_DEBUG(free(text_byte_code);)

    if (!byte_code) {
        PRINT_LINE();
        fprintf_s(stderr, "calloc failed\n");
        CLEAR_RESOURCES();
        return errno;
    }
    if (!text_byte_code) {
        PRINT_LINE();
        fprintf_s(stderr, "calloc failed\n");
        CLEAR_RESOURCES();
        return errno;
    }

    char cur_command[ASM_COMMAND_MAX_LEN + 1] = {};
    while (cur_len      < BYTE_CODE_MAX_LEN ON_DEBUG(and
           text_cur_len < BYTE_CODE_MAX_LEN)) {
        if (fscanf_s(code_stream, "%s", cur_command, ASM_COMMAND_MAX_LEN + 1) != 1) {
            PRINT_LINE();
            fprintf_s(stderr, "fscanf_s failed\n");
            CLEAR_RESOURCES();
            return ferror(code_stream);
        }

        //fprintf_s(stderr, "cur_len = %zu\ncur_command = %s\n", cur_len, cur_command);
        if (!strcmp(cur_command, "HLT")) {
            byte_code[cur_len++] = HLT_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhX", HLT_COMMAND);)

            if (fwrite(&cur_len, sizeof(size_t), 1, byte_code_stream) != 1) {
                PRINT_LINE();
                fprintf_s(stderr, "fwrite failed\n");
                CLEAR_RESOURCES();
                return ferror(byte_code_stream);
            }
            if (fwrite(byte_code, sizeof(byte_elem_t), cur_len, byte_code_stream) != cur_len) {
                PRINT_LINE();
                fprintf_s(stderr, "fwrite failed\n");
                CLEAR_RESOURCES();
                return ferror(byte_code_stream);
            }
            ON_DEBUG(fprintf_s(text_byte_code_stream, "%s", text_byte_code));
            CLEAR_RESOURCES();
            return 0;
        }

        if (!strcmp(cur_command, "PUSH")) {
            byte_code[cur_len++] = PUSH_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
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
                return BYTE_CODE_TOO_LONG;
            }
            *(assembler_elem_t *)(byte_code + cur_len) = a;
            cur_len += sizeof(assembler_elem_t);
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               assembler_elem_frm, a);)
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "\n");)
            continue;
        }

        if (!strcmp(cur_command, "PUSHR")) {
            byte_code[cur_len++] = PUSHR_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhX ", PUSHR_COMMAND);)

            byte_elem_t reg = 0;
            if (fscanf_s(code_stream, " r%hhu", &reg) != 1) {
                PRINT_LINE();
                fprintf_s(stderr, "fscanf_s failed");
                CLEAR_RESOURCES();
                return ferror(code_stream);
            }

            if (cur_len == BYTE_CODE_MAX_LEN) {
                CLEAR_RESOURCES();
                return BYTE_CODE_TOO_LONG;
            }

            if (reg < SPU_REGS_NUM) {
                byte_code[cur_len++] = reg;
                ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                                   BYTE_CODE_MAX_LEN - text_cur_len,
                                                   "%hhX\n", reg);)
                continue;
            }
            else {
                return INVALID_REGISTER;
            }
        }

        if (!strcmp(cur_command, "POP")) {
            byte_code[cur_len++] = POP_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhX ", POP_COMMAND);)
            continue;
        }

        if (!strcmp(cur_command, "POPR")) {
            byte_code[cur_len++] = POPR_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhX ", POPR_COMMAND);)

            byte_elem_t reg = 0;
            if (fscanf_s(code_stream, " r%hhu", &reg) != 1) {
                PRINT_LINE();
                fprintf_s(stderr, "fscanf_s failed\n");
                CLEAR_RESOURCES();
                return ferror(code_stream);
            }

            if (cur_len == BYTE_CODE_MAX_LEN) {
                CLEAR_RESOURCES();
                return BYTE_CODE_TOO_LONG;
            }

            if (reg < SPU_REGS_NUM) {
                byte_code[cur_len++] = reg;
                ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                                   BYTE_CODE_MAX_LEN - text_cur_len,
                                                   "%hhX\n", reg);)
                continue;
            }
            else {
                return INVALID_REGISTER;
            }
        }

        if (!strcmp(cur_command, "ADD")) {
            byte_code[cur_len++] = ADD_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhX\n", ADD_COMMAND);)
            continue;
        }

        if (!strcmp(cur_command, "SUB")) {
            byte_code[cur_len++] = SUB_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhX\n", SUB_COMMAND);)
            continue;
        }

        if (!strcmp(cur_command, "MLT")) {
            byte_code[cur_len++] = MLT_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhX\n", MLT_COMMAND);)
            continue;
        }

        if (!strcmp(cur_command, "DIV")) {
            byte_code[cur_len++] = DIV_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhX\n", DIV_COMMAND);)
            continue;
        }

        if (!strcmp(cur_command, "SQRT")) {
            byte_code[cur_len++] = SQRT_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhX\n", SQRT_COMMAND);)
            continue;
        }

        if (!strcmp(cur_command, "POW")) {
            byte_code[cur_len++] = POW_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhX\n", POW_COMMAND);)
            continue;
        }

        if (!strcmp(cur_command, "IN")) {
            byte_code[cur_len++] = IN_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhX\n", IN_COMMAND);)
            continue;
        }

        if (!strcmp(cur_command, "OUT")) {
            byte_code[cur_len++] = OUT_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhX\n", OUT_COMMAND);)
            continue;
        }

        if (!strcmp(cur_command, "JMP")) {
            byte_code[cur_len++] = JMP_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhX\n", JMP_COMMAND);)

            byte_elem_t pos = 0;
            if (fscanf_s(code_stream, "%hhu", &pos) != 1) {
                PRINT_LINE();
                fprintf_s(stderr, "fscanf_s failed\n");
                CLEAR_RESOURCES();
                return ferror(code_stream);
            }

            if (cur_len == BYTE_CODE_MAX_LEN) {
                CLEAR_RESOURCES();
                return BYTE_CODE_TOO_LONG;
            }

            byte_code[cur_len++] = pos;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhu\n", pos);)
            continue;
        }

        if (!strcmp(cur_command, "JB")) {
            byte_code[cur_len++] = JB_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhX\n", JB_COMMAND);)

            byte_elem_t pos = 0;
            if (fscanf_s(code_stream, "%hhu", &pos) != 1) {
                PRINT_LINE();
                fprintf_s(stderr, "fscanf_s failed\n");
                CLEAR_RESOURCES();
                return ferror(code_stream);
            }

            if (cur_len == BYTE_CODE_MAX_LEN) {
                CLEAR_RESOURCES();
                return BYTE_CODE_TOO_LONG;
            }

            byte_code[cur_len++] = pos;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhu\n", pos);)
            continue;
        }

        if (!strcmp(cur_command, "JBE")) {
            byte_code[cur_len++] = JBE_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhX\n", JBE_COMMAND);)

            byte_elem_t pos = 0;
            if (fscanf_s(code_stream, "%hhu", &pos) != 1) {
                PRINT_LINE();
                fprintf_s(stderr, "fscanf_s failed\n");
                CLEAR_RESOURCES();
                return ferror(code_stream);
            }

            if (cur_len == BYTE_CODE_MAX_LEN) {
                CLEAR_RESOURCES();
                return BYTE_CODE_TOO_LONG;
            }

            byte_code[cur_len++] = pos;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhu\n", pos);)
            continue;
        }

        if (!strcmp(cur_command, "JA")) {
            byte_code[cur_len++] = JA_COMMAND;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhX\n", JA_COMMAND);)

            byte_elem_t pos = 0;
            if (fscanf_s(code_stream, "%hhu", &pos) != 1) {
                PRINT_LINE();
                fprintf_s(stderr, "fscanf_s failed\n");
                CLEAR_RESOURCES();
                return ferror(code_stream);
            }

            if (cur_len == BYTE_CODE_MAX_LEN) {
                CLEAR_RESOURCES();
                return BYTE_CODE_TOO_LONG;
            }

            byte_code[cur_len++] = pos;
            ON_DEBUG(text_cur_len += sprintf_s(text_byte_code + text_cur_len,
                                               BYTE_CODE_MAX_LEN - text_cur_len,
                                               "%hhu\n", pos);)
            continue;
        }



        CLEAR_RESOURCES();
        return UNKNOWN_ASM_COMMAND;
    }

    CLEAR_RESOURCES();
    return BYTE_CODE_TOO_LONG;
}


