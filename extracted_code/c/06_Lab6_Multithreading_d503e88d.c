/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\06_Lab6_Multithreading.md
 * Line: 462
 * Language: c
 * Block ID: d503e88d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 非线程安全的计数器
int counter = 0;

void unsafe_increment() {
    counter++;  // 这不是原子操作！
}

// 实际执行的指令:
// 1. ld  t0, counter   (加载)
// 2. addi t0, t0, 1    (加1)
// 3. sd  t0, counter   (存储)

// 两个线程并发执行:
// T1: load(0)    T2: load(0)
// T1: add(1)     T2: add(1)
// T1: store(1)   T2: store(1)
// 结果: counter = 1 (期望是2)
