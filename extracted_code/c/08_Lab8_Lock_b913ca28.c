/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\08_Lab8_Lock.md
 * Line: 325
 * Language: c
 * Block ID: b913ca28
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/lockorder.h

// 定义全局锁顺序
enum lock_order {
    LOCK_MIN = 0,

    LOCK_BCACHE,      // 缓冲区缓存锁
    LOCK_VNODE,       // 虚拟节点锁
    LOCK_INODE,       // inode锁
    LOCK_DIR,         // 目录锁
    LOCK_FILE,        // 文件锁
    LOCK_PROC,        // 进程锁

    LOCK_MAX
};

// 每个锁关联一个顺序值
struct ordered_lock {
    struct spinlock lk;
    enum lock_order order;
};

// 按顺序获取多个锁
void
acquire_ordered(struct ordered_lock *lk1, struct ordered_lock *lk2)
{
    // 总是先获取顺序值小的锁
    if (lk1->order < lk2->order) {
        acquire(&lk1->lk);
        acquire(&lk2->lk);
    } else if (lk1->order > lk2->order) {
        acquire(&lk2->lk);
        acquire(&lk1->lk);
    } else {
        panic("acquire_ordered: same order");
    }
}
