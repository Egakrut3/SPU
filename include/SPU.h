#ifndef SPU_H
#define SPU_H

#include "Common.h"
#include "My_stack.h"

#ifdef _DEBUG
size_t const SPU_CANARY_NUM = 1;
#else
size_t const SPU_CANARY_NUM = 0;
#endif
size_t const CANARY = 0XFACE'FACE'FACE'FACE;

uint64_t const SPU_START_HASH   = 68901; static_assert(SPU_START_HASH & 1);
uint64_t const SPU_HASH_MLT     = 43;    static_assert(SPU_HASH_MLT & 1);

size_t const REGS_NUM = 4;

struct SPU {
    size_t            beg_canary[SPU_CANARY_NUM];

    My_stack          stack;
    ON_DEBUG(Var_info var_info;)
    byte_elem_t       *byte_code,   //TODO - possible make canary
                      *cur_command;
    ON_DEBUG(uint64_t hash_val;)
    stack_elem_t      regs[REGS_NUM];

    bool              is_valid;

    size_t            end_canary[SPU_CANARY_NUM];
};

errno_t execute_byte_code(User_error *error_ptr, FILE *byte_code_stream);

#endif
