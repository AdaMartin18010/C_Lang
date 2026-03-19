/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\02_Lab2_System_Calls.md
 * Line: 255
 * Language: c
 * Block ID: 9e1b2a9e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/kalloc.c
struct run {
    struct run *next;
};

struct {
    struct spinlock lock;
    struct run *freelist;
} kmem;

// 新增：统计空闲内存
uint64
count_freemem(void)
{
    uint64 count = 0;
    struct run *r;

    acquire(&kmem.lock);
    r = kmem.freelist;
    while (r) {
        count += PGSIZE;
        r = r->next;
    }
    release(&kmem.lock);

    return count;
}
