/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\02_Memory_Models_Matrix.md
 * Line: 162
 * Language: c
 * Block ID: f03265b6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 方式
_Atomic int flag = 0;
int data = 0;

void writer(void) {
    data = 42;
    atomic_store(&flag, 1, memory_order_release);
}

void reader(void) {
    while (atomic_load(&flag, memory_order_acquire) == 0);
    // data 保证为 42
    printf("%d\n", data);
}

// Linux内核方式
int flag = 0;
int data = 0;

void writer(void) {
    data = 42;
    smp_wmb();  // 写内存屏障
    WRITE_ONCE(flag, 1);
}

void reader(void) {
    while (READ_ONCE(flag) == 0);
    smp_rmb();  // 读内存屏障
    printf("%d\n", data);
}
