/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\06_Lab6_Multithreading.md
 * Line: 78
 * Language: c
 * Block ID: 0d5e3f17
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/uthread.c

// 线程上下文 - 保存所有callee-saved寄存器
// 加上ra和sp
struct context {
    uint64 ra;
    uint64 sp;

    // callee-saved寄存器
    uint64 s0;
    uint64 s1;
    uint64 s2;
    uint64 s3;
    uint64 s4;
    uint64 s5;
    uint64 s6;
    uint64 s7;
    uint64 s8;
    uint64 s9;
    uint64 s10;
    uint64 s11;
};

// 线程结构
struct thread {
    char       stack[STACK_SIZE];  // 线程栈
    struct context context;         // 保存的上下文
    int        state;               // 线程状态
};

enum { FREE, RUNNING, RUNNABLE };

struct thread threads[MAX_THREAD];
struct thread *current_thread;
