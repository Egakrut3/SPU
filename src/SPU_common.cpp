#include "My_stack.h"

uint64_t My_stack_hash(My_stack const *const stack_ptr) {
    assert(stack_ptr);

    uint64_t cur_hash = STACK_START_HASH;
#undef FINAL_CODE
#define FINAL_CODE

    ON_DEBUG(cur_hash = cur_hash * STACK_HASH_MLT +
                        (uint64_t)stack_ptr->var_info.position.file_name;)
    ON_DEBUG(cur_hash = cur_hash * STACK_HASH_MLT +
                        (uint64_t)stack_ptr->var_info.position.function_name;)
    ON_DEBUG(cur_hash = cur_hash * STACK_HASH_MLT +
                        stack_ptr->var_info.position.line;)
    ON_DEBUG(cur_hash = cur_hash * STACK_HASH_MLT +
                        (uint64_t)stack_ptr->var_info.name;)

    cur_hash = cur_hash * STACK_HASH_MLT + stack_ptr->size;
    cur_hash = cur_hash * STACK_HASH_MLT + stack_ptr->capacity;
    cur_hash = cur_hash * STACK_HASH_MLT + (uint64_t)stack_ptr->buffer;
    cur_hash = cur_hash * STACK_HASH_MLT + stack_ptr->is_valid;

    for (size_t i = 0; i < stack_ptr->size; ++i) {
        cur_hash = cur_hash * STACK_HASH_MLT + *(uint64_t *)&stack_ptr->buffer[i];
    }

    CLEAR_RESOURCES();
    return cur_hash;
}
