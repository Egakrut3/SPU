#ifndef MY_STACK_H
#define MY_STACK_H

#include "Common.h"

typedef int stack_elem_t;
char const stack_elem_str[] = "int",
           stack_elem_frm[] = "%d";

size_t const CANARY_SIZE = 2,
             CANARY_NUM = 1;
char const CANARY_PARTS[CANARY_SIZE] = {
    (char)0XFA,
    (char)0XCE,
};

struct My_stack {
    char              canary_beg[CANARY_NUM][CANARY_SIZE];

    size_t            size,
                      capacity;
    stack_elem_t      *buffer;
    ON_DEBUG(Var_info var_info;)

    bool              is_valid;

    char              canary_end[CANARY_NUM][CANARY_SIZE];
};

errno_t My_stack_Ctor(My_stack *stack_ptr, size_t start_capacity
                      ON_DEBUG(, Var_info var_info));

#ifdef _DEBUG
#define STACK_CREATE(name, start_capacity)                                                              \
My_stack name = {};                                                                                     \
CHECK_FUNC(My_stack_Ctor, &name, start_capacity,                                                        \
           Var_info{Position_info{__FILE__, __func__, __LINE__}, #name})
#else
#define STACK_CREATE(name, start_capacity)          \
My_stack name = {};                                 \
CHECK_FUNC(My_stack_Ctor, &name, start_capacity)
#endif

void My_stack_Dtor(My_stack *stack_ptr);

#define ATTEMPT_TO_ACCESS_TOP_FROM_EMPTY 1000

#define STACK_CANARY_SPOILED             0B10000000000
#define STACK_INVALID                    0B100000000000
#define STACK_NULL_CAPACITY              0B1000000000000
#define STACK_SIZE_GREATER_THAN_CAPACITY 0B10000000000000
#define STACK_NULL_BUFFER                0B100000000000000
#define STACK_BUFFER_CANARY_SPOILED      0B1000000000000000
#define STACK_BUFFER_SPOILED             0B10000000000000000 //TODO -
errno_t My_stack_verify(My_stack const *stack_ptr);

#define STACK_DUMP(out_stream, name, error)                                             \
My_stack_dump(out_stream, &name, Position_info{__FILE__, __func__, __LINE__}, error)
void My_stack_dump(FILE *out_stream, My_stack const *stack_ptr,
                   Position_info from_where, errno_t err);

errno_t My_stack_push(My_stack *stack_ptr, stack_elem_t elem);

errno_t My_stack_pop(My_stack *stack_ptr, stack_elem_t *dest);

errno_t My_stack_top(My_stack const *stack_ptr, stack_elem_t *dest);

#endif
