/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\02_Lock_Free_Programming.md
 * Line: 291
 * Language: c
 * Block ID: c41136e9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 位操作 - 原子标志位 */
typedef struct {
    _Atomic(uint32_t) flags;
} atomic_flags_t;

#define FLAG_A (1u << 0)
#define FLAG_B (1u << 1)
#define FLAG_C (1u << 2)

/* 设置标志 */
void set_flag(atomic_flags_t* f, uint32_t flag) {
    atomic_fetch_or_explicit(&f->flags, flag, memory_order_release);
}

/* 清除标志 */
void clear_flag(atomic_flags_t* f, uint32_t flag) {
    atomic_fetch_and_explicit(&f->flags, ~flag, memory_order_release);
}

/* 翻转标志 */
void toggle_flag(atomic_flags_t* f, uint32_t flag) {
    atomic_fetch_xor_explicit(&f->flags, flag, memory_order_release);
}

/* 检查并设置 */
bool test_and_set_flag(atomic_flags_t* f, uint32_t flag) {
    uint32_t old = atomic_fetch_or_explicit(&f->flags, flag,
                                             memory_order_acq_rel);
    return (old & flag) != 0;
}
