/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\01_C11_Memory_Model.md
 * Line: 98
 * Language: c
 * Block ID: dfc68ecd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 仅保证操作的原子性，不提供内存序保证 */
_Atomic(int) counter = 0;

void increment_relaxed(void) {
    /* 纯计数场景，不需要同步 */
    atomic_fetch_add_explicit(&counter, 1, memory_order_relaxed);
}

/* 使用场景:
 * - 纯计数器 (统计、ID生成)
 * - 不需要与其他变量同步的场景
 * - 追求极致性能
 */

/* 示例: 统计访问次数 */
_Atomic(uint64_t) page_hits = 0;

void on_page_visit(void) {
    atomic_fetch_add_explicit(&page_hits, 1, memory_order_relaxed);
}

uint64_t get_page_hits(void) {
    return atomic_load_explicit(&page_hits, memory_order_relaxed);
}
