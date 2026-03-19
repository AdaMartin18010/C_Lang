/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\02_Lab2_System_Calls.md
 * Line: 392
 * Language: c
 * Block ID: 6c71fe9a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

struct proc {
    // ...
    pagetable_t pagetable;    // 用户页表
    struct trapframe *trapframe;  // 保存用户寄存器
    // ...
};

// myproc()获取当前进程
struct proc* myproc(void);
