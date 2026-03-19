/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\02_Cache_Line_Optimization.md
 * Line: 110
 * Language: c
 * Block ID: f8cee1bd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 伪共享示例 - 两个线程频繁修改同一缓存行
struct SharedData {
    int counter1;  // 线程1修改
    int counter2;  // 线程2修改
    // 两个变量在同一缓存行(64字节)内！
} data;

// 线程1
void thread1_work() {
    for (int i = 0; i < 1000000; i++) {
        data.counter1++;  // 使整个缓存行失效
    }
}

// 线程2
void thread2_work() {
    for (int i = 0; i < 1000000; i++) {
        data.counter2++;  // 使整个缓存行失效
    }
}
