/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\05_Lab5_Cow.md
 * Line: 123
 * Language: c
 * Block ID: 4fb5e929
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/kalloc.c

// 增加引用计数
void
increment_ref(uint64 pa)
{
    if (pa < KERNBASE || pa >= PHYSTOP)
        panic("increment_ref: invalid pa");

    acquire(&ref_lock);
    ref_count[PA2IDX(pa)]++;
    release(&ref_lock);
}

// 减少引用计数，如果为0则释放
void
decrement_ref(uint64 pa)
{
    if (pa < KERNBASE || pa >= PHYSTOP)
        panic("decrement_ref: invalid pa");

    acquire(&ref_lock);
    int idx = PA2IDX(pa);
    if (ref_count[idx] < 1)
        panic("decrement_ref: ref_count < 1");

    ref_count[idx]--;
    int new_count = ref_count[idx];
    release(&ref_lock);

    // 如果引用为0，释放页面
    if (new_count == 0) {
        kfree((void *)pa);
    }
}

// 获取引用计数
int
get_ref(uint64 pa)
{
    if (pa < KERNBASE || pa >= PHYSTOP)
        return 0;

    acquire(&ref_lock);
    int count = ref_count[PA2IDX(pa)];
    release(&ref_lock);
    return count;
}
