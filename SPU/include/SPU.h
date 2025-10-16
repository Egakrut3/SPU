#ifndef SPU_H
#define SPU_H

#include "Common.h"
#include "My_stack.h"
#include "Assembler.h"

#ifdef _DEBUG
size_t const SPU_CANARY_NUM = 1;
#else
size_t const SPU_CANARY_NUM = 0;
#endif

uint64_t const SPU_START_HASH   = 5683; static_assert(SPU_START_HASH & 1);
uint64_t const SPU_HASH_MLT     = 985;  static_assert(SPU_HASH_MLT & 1);

size_t const SPU_REGS_NUM = 4;

struct SPU {
    size_t            beg_canary[SPU_CANARY_NUM];

    My_stack          stack;
    ON_DEBUG(Var_info var_info;)
    size_t            byte_code_len;
    byte_elem_t       *byte_code;    //TODO - possible make canary
    ON_DEBUG(uint64_t hash_val;)
    stack_elem_t      regs[SPU_REGS_NUM];

    bool              is_valid;

    size_t            end_canary[SPU_CANARY_NUM];
};

uint64_t SPU_hash(SPU const *SPU_ptr);

#define STACK_NOT_EMPTY_AFTER_EXECUTION 1'005
#define UNKNOWN_COMMAND                 1'006
#define NO_HLT_COMMAND_REACHED          1'007

errno_t SPU_Ctor(SPU *SPU_ptr, size_t start_capacity, FILE *byte_code_stream
                 ON_DEBUG(, Var_info var_info));

#ifdef _DEBUG
#define SPU_CREATE(name, start_capacity, byte_code_stream, handler)     \
SPU name = {};                                                          \
handler(SPU_Ctor, &name, start_capacity, byte_code_stream,              \
        Var_info{Position_info{__FILE__, __func__, __LINE__}, #name})
#else
#define SPU_CREATE(name, start_capacity, byte_code_stream, handler)     \
SPU name = {};                                                          \
handler(SPU_Ctor, &name, start_capacity, byte_code_stream)
#endif

void SPU_Dtor(SPU *SPU_ptr);

#define SPU_HASH_UNMATCH          0B100000000000000000
#define SPU_CANARY_SPOILED        0B1000000000000000000
#define SPU_INVALID               0B10000000000000000000
#define STACK_NULL_BYTE_CODE      0B100000000000000000000
errno_t SPU_verify(SPU const *SPU_ptr);

void SPU_dump(FILE *out_stream, SPU const *SPU_ptr,
              Position_info from_where, errno_t err);

#define SPU_DUMP(out_stream, name, error)                                           \
SPU_dump(out_stream, name, Position_info{__FILE__, __func__, __LINE__}, error)

errno_t SPU_execute(SPU *SPU_ptr);

#endif
