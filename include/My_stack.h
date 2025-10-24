#ifndef MY_STACK_H
#define MY_STACK_H

#include "Common.h"

typedef double stack_elem_t;
char const stack_elem_str[] = "double";
#define STACK_ELEM_FRM "%lG"
#define STACK_CANARY_FRM "%llX"
#ifdef _DEBUG
size_t const STACK_CANARY_NUM = 1;
#else
size_t const STACK_CANARY_NUM = 0;
#endif

uint64_t const STACK_BIT_BUFFER_CANARY = 0XFACE'FACE'FACE'FACE;
stack_elem_t const STACK_BUFFER_CANARY = *(stack_elem_t const *)&STACK_BIT_BUFFER_CANARY;

size_t const STACK_EXPANSION          = 2; static_assert(STACK_EXPANSION > 1);
size_t const STACK_REDUCTION_CRITERIA = 4; static_assert(STACK_REDUCTION_CRITERIA > 1);
size_t const STACK_REDUCTION          = 2; static_assert(STACK_REDUCTION > 1 and
                                                         STACK_REDUCTION <= STACK_REDUCTION_CRITERIA);
size_t const STACK_MIN_CAPACITY       = 5; static_assert(STACK_MIN_CAPACITY > 0);

uint64_t const STACK_START_HASH   = 68901; static_assert(STACK_START_HASH & 1);
uint64_t const STACK_HASH_MLT     = 43;    static_assert(STACK_HASH_MLT & 1);

struct My_stack {
    size_t            beg_canary[STACK_CANARY_NUM];

    ON_DEBUG(Var_info var_info;)
    size_t            size,
                      capacity;
    stack_elem_t      *buffer;
    ON_DEBUG(uint64_t hash_val;)

    bool              is_valid;

    size_t            end_canary[STACK_CANARY_NUM];
};

uint64_t My_stack_hash(My_stack const *stack_ptr);

errno_t My_stack_Ctor(My_stack *stack_ptr, size_t start_capacity
                      ON_DEBUG(, Var_info var_info));

#ifdef _DEBUG
#define STACK_CREATE(name, start_capacity, handler)                     \
My_stack name = {};                                                     \
handler(My_stack_Ctor, &name, start_capacity,                           \
        Var_info{Position_info{__FILE__, __func__, __LINE__}, #name})
#else
#define STACK_CREATE(name, start_capacity, handler) \
My_stack name = {};                                 \
handler(My_stack_Ctor, &name, start_capacity)
#endif

void My_stack_Dtor(My_stack *stack_ptr);

#define STACK_HASH_UNMATCH               0B10'000'000'000
#define STACK_CANARY_SPOILED             0B100'000'000'000
#define STACK_INVALID                    0B1'000'000'000'000
#define STACK_NULL_CAPACITY              0B10'000'000'000'000
#define STACK_SIZE_GREATER_THAN_CAPACITY 0B100'000'000'000'000
#define STACK_NULL_BUFFER                0B1'000'000'000'000'000
#define STACK_BUFFER_CANARY_SPOILED      0B10'000'000'000'000'000
errno_t My_stack_verify(My_stack const *stack_ptr);

errno_t My_stack_dump(FILE *out_stream, My_stack const *stack_ptr,
                      errno_t err, Position_info from_where,
                      size_t tab_count, bool need_extra_info);

#define STACK_DUMP(out_stream, name, error, handler)                                                    \
handler(My_stack_dump, out_stream, &name, error, Position_info{__FILE__, __func__, __LINE__}, 0, true)

#define ATTEMPT_TO_ACCESS_TOP_FROM_EMPTY 2'000

errno_t My_stack_push(My_stack *stack_ptr, stack_elem_t elem);

errno_t My_stack_pop(My_stack *stack_ptr, stack_elem_t *dest);

errno_t My_stack_top(My_stack const *stack_ptr, stack_elem_t *dest);

#endif
