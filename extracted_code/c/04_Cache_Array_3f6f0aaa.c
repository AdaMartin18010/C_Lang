/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\03_Memory_Circuits\04_Cache_Array.md
 * Line: 1078
 * Language: c
 * Block ID: 3f6f0aaa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 多线程Cache一致性考虑
// ============================================================

// MESI协议在多核编程中的体现

// 场景1: 只读共享 (E → S)
const int shared_data = 42;  // 所有核的Cache为E或S状态

void read_only_access() {
    // 多个核读取shared_data
    // 第一次读取：E状态（独占）
    // 第二次读取：变为S状态（共享）
    int x = shared_data;
}

// 场景2: 写写冲突 (M → I)
int shared_counter = 0;

// 核0
void core0_increment() {
    shared_counter++;  // M状态，其他核的副本变为I
}

// 核1
void core1_increment() {
    shared_counter++;  // 需要先从核0获取最新值
}
/*
 * 性能问题：
 * - 频繁的M→I状态转换
 * - 大量Cache一致性流量
 * - 解决方案：使用线程本地变量或原子操作
 */

// 场景3: 使用原子操作优化
#include <stdatomic.h>

_Atomic int atomic_counter = 0;

void atomic_increment() {
    atomic_fetch_add(&atomic_counter, 1);
    // 硬件原子指令，配合Cache锁定
}

// 场景4: 内存屏障与Cache刷新
void producer_consumer() {
    int data = 0;
    volatile int flag = 0;

    // Producer (核0)
    data = 42;           // 写入数据
    __sync_synchronize(); // 内存屏障：确保写入顺序
    flag = 1;            // 设置标志

    // Consumer (核1)
    while (flag == 0);   // 等待标志
    __sync_synchronize(); // 内存屏障：确保读取顺序
    int value = data;    // 读取数据
}
/*
 * 内存屏障作用：
 * - 确保Store/Load顺序
 * - 触发必要的Cache刷新/失效
 * - 保证多核间可见性
 */
