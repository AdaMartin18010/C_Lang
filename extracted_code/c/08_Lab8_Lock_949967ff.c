/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\08_Lab8_Lock.md
 * Line: 93
 * Language: c
 * Block ID: 949967ff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/spinlock.h

struct spinlock {
    uint locked;       // 锁状态: 0=空闲, 1=被持有

    // 调试信息
    char *name;        // 锁名称
    struct cpu *cpu;   // 持有锁的CPU
};
