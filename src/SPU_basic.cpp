#include "SPU.h"

errno_t SPU_Ctor(SPU *const SPU_ptr, size_t const start_capacity, FILE *const byte_code_stream
                 ON_DEBUG(, Var_info const var_info)) {
    assert(SPU_ptr); assert(!SPU_ptr->is_valid); assert(start_capacity); assert(byte_code_stream);
    ON_DEBUG(assert(var_info.position.file_name); assert(var_info.name);)

#undef FINAL_CODE
#define FINAL_CODE

    for (size_t i = 0; i < SPU_CANARY_NUM; ++i) {
        SPU_ptr->beg_canary[i] = CANARY;
    }

    CHECK_FUNC(My_stack_Ctor, &SPU_ptr->stack, start_capacity, var_info);
    ON_DEBUG(SPU_ptr->var_info   = var_info;)

    if (fread(&SPU_ptr->byte_code_len, sizeof(SPU_ptr->byte_code_len), 1, byte_code_stream) != 1) {
        PRINT_LINE();
        fprintf_s(stderr, "fread failed\n");
        CLEAR_RESOURCES();
        return ferror(byte_code_stream);
    }
    if (SPU_ptr->byte_code_len) {
        byte_elem_t *const new_buffer = (byte_elem_t *)calloc(SPU_ptr->byte_code_len,
                                                              sizeof(byte_elem_t));
        if (!new_buffer) {
            PRINT_LINE();
            perror("calloc failed");
            CLEAR_RESOURCES();
            return errno;
        }

        SPU_ptr->byte_code = new_buffer;
        if (fread(SPU_ptr->byte_code, sizeof(byte_elem_t), SPU_ptr->byte_code_len, byte_code_stream) !=
                                                           SPU_ptr->byte_code_len) {
            PRINT_LINE();
            fprintf_s(stderr, "fread failed\n");
            CLEAR_RESOURCES();
            return ferror(byte_code_stream);
        }
    }
    else {
        PRINT_LINE();
        fprintf_s(stderr, "File contains empty byte-code\n");
        CLEAR_RESOURCES();
        return EMPTY_BYTE_CODE;
    }

    SPU_ptr->is_valid          = true;
    ON_DEBUG(SPU_ptr->hash_val = SPU_hash(SPU_ptr);)

    for (size_t i = 0; i < SPU_CANARY_NUM; ++i) {
        SPU_ptr->end_canary[i] = CANARY;
    }

    CLEAR_RESOURCES();
    return 0;
}

void SPU_Dtor(SPU *const SPU_ptr) {
    assert(SPU_ptr);

#undef FINAL_CODE
#define FINAL_CODE

    My_stack_Dtor(&SPU_ptr->stack);
    free(SPU_ptr->byte_code);
    SPU_ptr->is_valid = false;

    CLEAR_RESOURCES();
}

errno_t SPU_verify(SPU const *const SPU_ptr) {
    assert(SPU_ptr);

    errno_t err = 0;
#undef FINAL_CODE
#define FINAL_CODE

    err |= My_stack_verify(&SPU_ptr->stack);

    if (SPU_ptr->hash_val != SPU_hash(SPU_ptr)) {
        err |= SPU_HASH_UNMATCH;
    }

    for (size_t i = 0; i < SPU_CANARY_NUM; ++i) {
        if (SPU_ptr->beg_canary[i] != CANARY or
            SPU_ptr->end_canary[i] != CANARY) {
            err |= SPU_CANARY_SPOILED;
            break;
        }
    }

    if (!SPU_ptr->is_valid) {
        err |= SPU_INVALID;
    }

    if (!SPU_ptr->byte_code_len) {
        err |= SPU_NULL_BYTE_CODE_LEN;
    }

    if (!SPU_ptr->byte_code) {
        err |= STACK_NULL_BYTE_CODE;
    }

    CLEAR_RESOURCES();
    return err;
}

void SPU_dump(FILE *const out_stream, SPU const *const SPU_ptr,
              Position_info const from_where, errno_t const err) {
    assert(out_stream); assert(SPU_ptr);
    ON_DEBUG(assert(SPU_ptr->var_info.name); assert(SPU_ptr->var_info.position.file_name);)
    assert(from_where.file_name);

#undef FINAL_CODE
#define FINAL_CODE

    fprintf_s(out_stream, "called at file %s, line %d in \"%s\" function: ",
              from_where.file_name, from_where.line, from_where.function_name);

    if (err & SPU_HASH_UNMATCH) {
        fprintf_s(out_stream, "SPU hash unmatch    ");
    }

    if (err & SPU_CANARY_SPOILED) {
        fprintf_s(out_stream, "SPU canary spoiled    ");
    }

    if (err & SPU_INVALID) {
        fprintf_s(out_stream, "SPU invalid    ");
    }

    if (err & SPU_NULL_BYTE_CODE_LEN) {
        fprintf_s(out_stream, "SPU has null byte_code_len    ");
    }

    if (err & STACK_NULL_BYTE_CODE) {
        fprintf_s(out_stream, "SPU has null byte_code    ");
    }

    fprintf_s(out_stream, "\nSPU[%p]"
              ON_DEBUG(" \"%s\" declared in file %s, line %zu in \"%s\" function")
              " {\n",
              SPU_ptr
              ON_DEBUG(, SPU_ptr->var_info.name,
              SPU_ptr->var_info.position.file_name, SPU_ptr->var_info.position.line,
              SPU_ptr->var_info.position.function_name));

    fprintf_s(out_stream, "\tbeg_canary[%zu] = [%p] {\n", SPU_CANARY_NUM, SPU_ptr->beg_canary);
    for (size_t i = 0; i < SPU_CANARY_NUM; ++i) {
        fprintf_s(out_stream, "\t\t[%zu] = [%zX]\n", i, SPU_ptr->beg_canary[i]);
    }
    fprintf_s(out_stream, "\t}\n");

    My_stack_dump(out_stream, &SPU_ptr->stack, from_where, err); //TODO - I can't add tabulation there and mute comments

    fprintf_s(out_stream, "\tbyte_code_len = %zu\n", SPU_ptr->byte_code_len);

    fprintf_s(out_stream, "\tbyte_code[%zu] = [%p] {\n", SPU_ptr->byte_code_len, SPU_ptr->byte_code);
    for (size_t i = 0; i < SPU_ptr->byte_code_len; i += sizeof(stack_elem_t)) {
        fprintf_s(out_stream, "\t\t");
        for (size_t j = i; j < i + sizeof(stack_elem_t) and j < SPU_ptr->byte_code_len; ++j) {
            fprintf_s(out_stream, "[%zu] = %hhX ", j, SPU_ptr->byte_code[j]);
        }
        fprintf_s(out_stream, "\n");
    }
    fprintf_s(out_stream, "\t}\n");

    ON_DEBUG(fprintf_s(out_stream, "\thash_val = %llX, must be %llX\n", SPU_ptr->hash_val,
                                                                        SPU_hash(SPU_ptr)));
    fprintf_s(out_stream, "\tis_valid = %d\n", SPU_ptr->is_valid);

    fprintf_s(out_stream, "\tend_canary[%zu] = [%p] {\n", SPU_CANARY_NUM, SPU_ptr->end_canary);
    for (size_t i = 0; i < SPU_CANARY_NUM; ++i) {
        fprintf_s(out_stream, "\t\t[%zu] = [%zX]\n", i, SPU_ptr->end_canary[i]);
    }
    fprintf_s(out_stream, "\t}\n");

    fprintf_s(out_stream, "}\n");
    CLEAR_RESOURCES();
}
