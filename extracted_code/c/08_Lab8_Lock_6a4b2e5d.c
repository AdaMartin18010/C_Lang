/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\08_Lab8_Lock.md
 * Line: 226
 * Language: c
 * Block ID: 6a4b2e5d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/sleeplock.h

struct sleeplock {
    uint locked;       // 锁状态
    struct spinlock lk; // 保护此结构的自旋锁

    // 调试信息
    char *name;
    int pid;           // 持有锁的进程
};
