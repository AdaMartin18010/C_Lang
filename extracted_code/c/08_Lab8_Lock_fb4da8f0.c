/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\08_Lab8_Lock.md
 * Line: 367
 * Language: c
 * Block ID: fb4da8f0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/lockdebug.c

#ifdef DEBUG_LOCK

#define MAX_LOCK_DEPTH 10

struct lock_stack {
    struct spinlock *locks[MAX_LOCK_DEPTH];
    int depth;
};

struct lock_stack lock_stacks[NCPU];

void
lockdebug_acquire(struct spinlock *lk)
{
    struct lock_stack *ls = &lock_stacks[cpuid()];

    // 检查：锁是否已经被持有
    for (int i = 0; i < ls->depth; i++) {
        if (ls->locks[i] == lk) {
            panic("lockdebug: recursive acquire of %s", lk->name);
        }
    }

    // 检查：锁顺序
    for (int i = 0; i < ls->depth; i++) {
        if (ls->locks[i] > lk) {
            printf("WARNING: lock order violation\n");
            printf("  acquired %s after %s\n",
                   lk->name, ls->locks[i]->name);
        }
    }

    // 记录锁获取
    if (ls->depth >= MAX_LOCK_DEPTH)
        panic("lockdebug: lock stack overflow");

    ls->locks[ls->depth++] = lk;
}

void
lockdebug_release(struct spinlock *lk)
{
    struct lock_stack *ls = &lock_stacks[cpuid()];

    // 检查：释放的必须是最后获取的锁
    if (ls->depth == 0 || ls->locks[ls->depth - 1] != lk) {
        panic("lockdebug: release order violation");
    }

    ls->depth--;
}

void
lockdebug_dump(void)
{
    struct lock_stack *ls = &lock_stacks[cpuid()];

    printf("Lock stack (depth=%d):\n", ls->depth);
    for (int i = 0; i < ls->depth; i++) {
        printf("  %d: %s\n", i, ls->locks[i]->name);
    }
}

#endif // DEBUG_LOCK
