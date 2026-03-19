/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\04_Lab4_Traps.md
 * Line: 391
 * Language: c
 * Block ID: da99c6e0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/proc.h

struct proc {
    // ... 原有字段 ...

    // Alarm相关字段
    int alarm_interval;        // alarm间隔(ticks)
    void (*alarm_handler)();   // alarm处理函数
    int alarm_ticks;           // 距离下次alarm的ticks数
    int alarm_goingoff;        // 是否正在执行handler (防止重入)
    struct trapframe *alarm_trapframe;  // 保存的trapframe副本
};
