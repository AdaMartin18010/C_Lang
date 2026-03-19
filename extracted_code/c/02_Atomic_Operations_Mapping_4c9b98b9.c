/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 750
 * Language: c
 * Block ID: 4c9b98b9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 Lock-Free 检测
#include <stdatomic.h>

void check_lock_free(void) {
    #if ATOMIC_INT_LOCK_FREE == 2
    // int 总是 lock-free
    #endif

    _Atomic int var = ATOMIC_VAR_INIT(0);
    bool is_lock_free = atomic_is_lock_free(&var);
}
