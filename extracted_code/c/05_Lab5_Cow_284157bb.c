/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\05_Lab5_Cow.md
 * Line: 176
 * Language: c
 * Block ID: 284157bb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/kalloc.c

// 分配页面时设置引用计数为1
void *
kalloc(void)
{
    struct run *r;

    acquire(&kmem.lock);
    r = kmem.freelist;
    if (r) {
        kmem.freelist = r->next;

        // 设置引用计数为1
        uint64 pa = (uint64)r;
        ref_count[PA2IDX(pa)] = 1;
    }
    release(&kmem.lock);

    if (r)
        memset((char *)r, 5, PGSIZE);  // 填充垃圾值

    return (void *)r;
}

// kfree不再直接释放，而是调用decrement_ref
void
kfree(void *pa)
{
    // 添加到空闲列表 (原有逻辑)
    struct run *r = (struct run *)pa;

    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
}
