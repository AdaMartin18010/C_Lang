/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\08_Lab8_Lock.md
 * Line: 520
 * Language: c
 * Block ID: 5e1bf7aa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/rwlock.h

struct rwlock {
    struct spinlock lk;     // 保护此结构
    int readers;            // 读者数量
    int writing;            // 是否有写者
    struct proc *writer;    // 写者进程
    struct spinlock wait_lk; // 等待队列锁
};

void initrwlock(struct rwlock *rw, char *name);
void readlock(struct rwlock *rw);
void readunlock(struct rwlock *rw);
void writelock(struct rwlock *rw);
void writeunlock(struct rwlock *rw);
