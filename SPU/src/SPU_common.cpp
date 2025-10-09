#include "SPU.h"

uint64_t SPU_hash(SPU const *const SPU_ptr) {
    assert(SPU_ptr);

    uint64_t cur_hash = SPU_START_HASH;
#undef FINAL_CODE
#define FINAL_CODE

    cur_hash = cur_hash * SPU_HASH_MLT + My_stack_hash(&SPU_ptr->stack);

    ON_DEBUG(cur_hash = cur_hash * SPU_HASH_MLT +
                        (uint64_t)SPU_ptr->var_info.position.file_name;)
    ON_DEBUG(cur_hash = cur_hash * SPU_HASH_MLT +
                        (uint64_t)SPU_ptr->var_info.position.function_name;)
    ON_DEBUG(cur_hash = cur_hash * SPU_HASH_MLT +
                        SPU_ptr->var_info.position.line;)
    ON_DEBUG(cur_hash = cur_hash * SPU_HASH_MLT +
                        (uint64_t)SPU_ptr->var_info.name;)

    cur_hash = cur_hash * SPU_HASH_MLT + SPU_ptr->byte_code_len;
    cur_hash = cur_hash * SPU_HASH_MLT + (uint64_t)SPU_ptr->byte_code;
    for (size_t i = 0; i < SPU_REGS_NUM; ++i) {
        cur_hash = cur_hash * SPU_HASH_MLT + *(const uint64_t *)&SPU_ptr->regs[i];
    }
    cur_hash = cur_hash * SPU_HASH_MLT + SPU_ptr->is_valid;

    CLEAR_RESOURCES();
    return cur_hash;
}
