/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 374
 * Language: c
 * Block ID: 5b46f3b3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 Fetch-Op 操作
_Atomic int32_t counter = ATOMIC_VAR_INIT(0);
_Atomic uint64_t bitmap = ATOMIC_VAR_INIT(0);

void fetch_op_examples(void) {
    // 算术操作
    int32_t old1 = atomic_fetch_add(&counter, 1);
    int32_t old2 = atomic_fetch_sub(&counter, 1);

    // 位操作
    uint64_t old3 = atomic_fetch_or(&bitmap, 0xFF);
    uint64_t old4 = atomic_fetch_and(&bitmap, ~0xFF);
    uint64_t old5 = atomic_fetch_xor(&bitmap, 0x0F);
}
