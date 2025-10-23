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

size_t const SPU_MEM_SIZE_SQRT = 25;

struct SPU {
    size_t            beg_canary[SPU_CANARY_NUM];

    My_stack          calc_stack,
                      call_stack;
    ON_DEBUG(Var_info var_info;)
    size_t            byte_code_len;
    Assembler_elem    *byte_code;
    stack_elem_t      regs[REGS_NUM];
    stack_elem_t      memory[SPU_MEM_SIZE_SQRT * SPU_MEM_SIZE_SQRT];
    ON_DEBUG(uint64_t hash_val;)

    bool              is_valid;

    size_t            end_canary[SPU_CANARY_NUM];
};

uint64_t SPU_hash(SPU const *SPU_ptr);

#define INVALID_POSITION                3'000
#define NOT_ENOUGH_ARGUMENTS            3'001
#define INVALID_INDEX                   3'002
#define STACK_NOT_EMPTY_AFTER_EXECUTION 3'003
#define NO_HLT_COMMAND_REACHED          3'004

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

errno_t SPU_dump(FILE *out_stream, SPU const *SPU_ptr,
                 errno_t err, Position_info from_where,
                 size_t tab_count, bool need_extra_info);

#define SPU_DUMP(out_stream, name, error, handler)                                           \
handler(SPU_dump, out_stream, &name, error, Position_info{__FILE__, __func__, __LINE__}, 0, true)

errno_t SPU_execute(SPU *SPU_ptr);

#endif
