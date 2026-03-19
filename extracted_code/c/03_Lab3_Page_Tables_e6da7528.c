/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\03_Lab3_Page_Tables.md
 * Line: 202
 * Language: c
 * Block ID: e6da7528
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/proc.h
struct usyscall {
    int pid;  // 进程ID (用户态只读)
};

struct proc {
    // ...
    struct usyscall *usyscall;  // USYSCALL页
};

// kernel/proc.c
// 在allocproc()中创建USYSCALL页
static struct proc* allocproc(void) {
    // ...

    // 分配USYSCALL页
    if ((p->usyscall = (struct usyscall *)kalloc()) == 0) {
        freeproc(p);
        release(&p->lock);
        return 0;
    }

    // 初始化pid
    p->usyscall->pid = p->pid;

    // 映射到用户态地址空间 (USYSCALL = 0x40000000)
    if (mappages(p->pagetable, USYSCALL, PGSIZE,
                 (uint64)(p->usyscall), PTE_R | PTE_U) < 0) {
        freeproc(p);
        release(&p->lock);
        return 0;
    }

    // ...
}

// 用户库 (user/usys.pl)
// 修改getpid()实现
// 如果USYSCALL页可用，直接读取，否则使用系统调用
