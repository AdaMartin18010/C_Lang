/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\01_C11_Memory_Model.md
 * Line: 164
 * Language: c
 * Block ID: 36384fcc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 默认、最强的内存序，所有线程看到一致的操作顺序 */
_Atomic(int) x = 0;
_Atomic(int) y = 0;

void thread1(void) {
    atomic_store(&x, 1);  /* seq_cst */
    atomic_store(&y, 1);  /* seq_cst */
}

void thread2(void) {
    int a = atomic_load(&y);  /* seq_cst */
    int b = atomic_load(&x);  /* seq_cst */

    /* 不可能出现: a=1, b=0 的情况
     * 因为所有seq_cst操作有全局顺序
     */
}

/* 使用场景:
 * - 默认选择，最安全
 * - 锁的实现
 * - 需要全局顺序保证的场景
 *
 * 性能: 比acquire-release慢，因为需要额外的内存屏障
 */
