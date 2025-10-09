#include "Disassembler.h"

static size_t const CODE_MAX_LEN = 0X1000;

errno_t disassembly_byte_code(FILE *byte_code_stream, FILE *code_stream) {
    assert(byte_code_stream); assert(code_stream);

    size_t cur_len = 0;
    char *const code = (char *)calloc(CODE_MAX_LEN + 1, sizeof(char));
#undef FINAL_CODE
#define FINAL_CODE  \
    free(code);

    if (!code) {
        PRINT_LINE();
        fprintf_s(stderr, "calloc failed\n");
        CLEAR_RESOURCES();
        return errno;
    }

    size_t byte_code_len = 0;
    if (fread(&byte_code_len, sizeof(size_t), 1, byte_code_stream) != 1) {
        PRINT_LINE();
        fprintf_s(stderr, "fread failed\n");
        CLEAR_RESOURCES();
        return ferror(byte_code_stream);
    }
    //fprintf_s(stderr, "byte_code_len = %zu\n", byte_code_len);
    char *const byte_code = (char *)calloc(byte_code_len, sizeof(char));

#undef FINAL_CODE
#define FINAL_CODE      \
    free(code);         \
    free(byte_code);

    if (!code) {
        PRINT_LINE();
        fprintf_s(stderr, "calloc failed\n");
        CLEAR_RESOURCES();
        return errno;
    }
    if (fread(byte_code, sizeof(char), byte_code_len, byte_code_stream) != byte_code_len) {
        PRINT_LINE();
        fprintf_s(stderr, "fread failed\n");
        CLEAR_RESOURCES();
        return ferror(byte_code_stream);
    }

    stack_elem_t op  = {};
    byte_elem_t reg1 = {},
                pos  = {};
    for (size_t IC = 0; IC < byte_code_len and cur_len < CODE_MAX_LEN; ++IC) {
        //fprintf_s(stderr, "IC = %zu\ncur_len = %zu\ncur_command = %hhu\n", IC, cur_len, byte_code[IC]);
        switch (byte_code[IC]) {
            case HLT_COMMAND:
                cur_len += sprintf_s(code + cur_len, CODE_MAX_LEN - cur_len, "HLT");
                fprintf_s(code_stream, "%s", code);
                CLEAR_RESOURCES();
                return 0;

            case PUSH_COMMAND:
                IC = get_assembler_aligned(IC + 1);
                if (IC < byte_code_len) {
                    op = *(stack_elem_t *)(byte_code + IC);
                    cur_len += sprintf_s(code + cur_len, CODE_MAX_LEN - cur_len, "PUSH ");
                    cur_len += sprintf_s(code + cur_len, CODE_MAX_LEN - cur_len, stack_elem_frm, op);
                    cur_len += sprintf_s(code + cur_len, CODE_MAX_LEN - cur_len, "\n");
                    IC += sizeof(stack_elem_t) - 1;
                    break;
                }
                else { CLEAR_RESOURCES(); return INCOMPLETE_ARGUMENT; }

            case PUSHR_COMMAND:
                ++IC;
                if (IC < byte_code_len) {
                    reg1 = byte_code[IC];
                    cur_len += sprintf_s(code + cur_len, CODE_MAX_LEN - cur_len, "PUSHR r%hhu\n", reg1);
                    break;
                }
                else { CLEAR_RESOURCES(); return INCOMPLETE_ARGUMENT; }

            case POP_COMMAND:
                cur_len += sprintf_s(code + cur_len, CODE_MAX_LEN - cur_len, "POP\n");
                break;

            case POPR_COMMAND:
                ++IC;
                if (IC < byte_code_len) {
                    reg1 = byte_code[IC];
                    cur_len += sprintf_s(code + cur_len, CODE_MAX_LEN - cur_len, "POPR r%hhu\n", reg1);
                    break;
                }
                else { CLEAR_RESOURCES(); return INCOMPLETE_ARGUMENT; }

            case ADD_COMMAND:
                cur_len += sprintf_s(code + cur_len, CODE_MAX_LEN - cur_len, "ADD\n");
                break;

            case SUB_COMMAND:
                cur_len += sprintf_s(code + cur_len, CODE_MAX_LEN - cur_len, "SUB\n");
                break;

            case MLT_COMMAND:
                cur_len += sprintf_s(code + cur_len, CODE_MAX_LEN - cur_len, "MLT\n");
                break;

            case DIV_COMMAND:
                cur_len += sprintf_s(code + cur_len, CODE_MAX_LEN - cur_len, "DIV\n");
                break;

            case SQRT_COMMAND:
                cur_len += sprintf_s(code + cur_len, CODE_MAX_LEN - cur_len, "SQRT\n");
                break;

            case POW_COMMAND:
                cur_len += sprintf_s(code + cur_len, CODE_MAX_LEN - cur_len, "POW\n");
                break;

            case IN_COMMAND:
                cur_len += sprintf_s(code + cur_len, CODE_MAX_LEN - cur_len, "IN\n");
                break;

            case OUT_COMMAND:
                cur_len += sprintf_s(code + cur_len, CODE_MAX_LEN - cur_len, "OUT\n");
                break;

            case JMP_COMMAND:
                ++IC;
                if (IC < byte_code_len) {
                    pos = byte_code[IC];
                    cur_len += sprintf_s(code + cur_len, CODE_MAX_LEN - cur_len, "JMP %hhu\n", pos);
                    break;
                }
                else { CLEAR_RESOURCES(); return INCOMPLETE_ARGUMENT; }

            default:
                CLEAR_RESOURCES();
                return UNKNOWN_COMMAND;
        }
    }

    CLEAR_RESOURCES();
    return CODE_TOO_LONG;
}
