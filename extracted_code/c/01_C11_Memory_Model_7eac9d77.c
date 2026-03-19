/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\01_C11_Memory_Model.md
 * Line: 194
 * Language: c
 * Block ID: 7eac9d77
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 双向同步 - 用于读写锁、引用计数等 */
_Atomic(int) refcount = 1;

void retain(void) {
    /* 增加引用计数 */
    atomic_fetch_add_explicit(&refcount, 1, memory_order_relaxed);
}

bool release(void) {
    /* Acq-Rel: 获取之前的操作，释放后续操作 */
    int old = atomic_fetch_sub_explicit(&refcount, 1,
                                        memory_order_acq_rel);
    if (old == 1) {
        /* 最后一个引用，可以安全释放 */
        return true;
    }
    return false;
}

/* 使用场景:
 * - 引用计数
 * - 读写锁
 * - 需要读写双向同步的场景
 */
