/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\02_Lab2_System_Calls.md
 * Line: 105
 * Language: c
 * Block ID: ac30acc0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/proc.h
struct proc {
    struct spinlock lock;

    // p->lock must be held when using these:
    enum procstate state;
    void *chan;
    int killed;
    int xstate;
    int pid;

    // wait_lock must be held when using this:
    struct proc *parent;

    // these are private to the process, so p->lock need not be held.
    uint64 kstack;
    uint64 sz;
    pagetable_t pagetable;
    struct trapframe *trapframe;
    struct context context;
    struct file *ofile[NOFILE];
    struct inode *cwd;
    char name[16];

    // 新增：跟踪掩码
    int trace_mask;
};
