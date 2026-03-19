/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\01_C11_Memory_Model.md
 * Line: 374
 * Language: c
 * Block ID: 61a32b5b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

_Atomic(int) flag = 0;
int data = 0;

void thread_a(void) {
    data = 42;  /* A */
    atomic_store_explicit(&flag, 1, memory_order_release);  /* B */
}

void thread_b(void) {
    while (atomic_load_explicit(&flag, memory_order_acquire) != 1) {
        /* 自旋 */
    }  /* C */

    /* D: 这里看到 A 的写入，因为 B release -> C acquire */
    printf("%d\n", data);  /* 保证输出 42 */
}

/* Happens-Before 链:
 * A -> B (同一线程)
 * B -> C (release-acquire 同步)
 * C -> D (同一线程)
 * 因此: A -> D，data的写入对读取可见
 */
