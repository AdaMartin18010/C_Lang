/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\02_Atomic_Operations.md
 * Line: 413
 * Language: c
 * Block ID: c78f99a4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：用relaxed实现同步
atomic_int flag = 0;
int data = 0;

void producer() {
    data = 42;
    atomic_store(&flag, 1, memory_order_relaxed);  // 太弱！
}

void consumer() {
    while (atomic_load(&flag, memory_order_relaxed) == 0);
    printf("%d\n", data);  // 可能读到0！
}

// 正确：使用acquire-release
atomic_store(&flag, 1, memory_order_release);
while (atomic_load(&flag, memory_order_acquire) == 0);
