/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\08_Lab8_Lock.md
 * Line: 105
 * Language: c
 * Block ID: 5e9b9304
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/spinlock.c

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "proc.h"
#include "defs.h"

void
initlock(struct spinlock *lk, char *name)
{
    lk->name = name;
    lk->locked = 0;
    lk->cpu = 0;
}

// 获取自旋锁
void
acquire(struct spinlock *lk)
{
    // 关中断，防止中断处理程序死锁
    push_off();

    // 检查：不能在持有锁时再次获取
    if (holding(lk))
        panic("acquire");

    // 自旋直到获取锁
    // 使用amoswap实现test-and-set
    while (__sync_lock_test_and_set(&lk->locked, 1) != 0)
        ;

    // 内存屏障：确保锁获取在临界区之前完成
    __sync_synchronize();

    // 记录持有信息
    lk->cpu = mycpu();
}

// 释放自旋锁
void
release(struct spinlock *lk)
{
    // 检查：必须持有锁才能释放
    if (!holding(lk))
        panic("release");

    lk->cpu = 0;

    // 内存屏障：确保临界区在锁释放之前完成
    __sync_synchronize();

    // 释放锁
    __sync_lock_release(&lk->locked);

    // 开中断
    pop_off();
}

// 检查是否持有锁
int
holding(struct spinlock *lk)
{
    int r;
    r = (lk->locked && lk->cpu == mycpu());
    return r;
}
