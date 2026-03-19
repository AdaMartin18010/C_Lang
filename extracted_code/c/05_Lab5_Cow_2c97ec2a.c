/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\05_Lab5_Cow.md
 * Line: 319
 * Language: c
 * Block ID: 2c97ec2a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/trap.c (或 vm.c)

int
cow_handler(uint64 va)
{
    struct proc *p = myproc();
    pte_t *pte;
    uint64 pa;
    uint flags;
    char *new_page;

    // 检查虚拟地址合法性
    if (va >= p->sz || va < PGSIZE)  // 跳过NULL指针
        return -1;

    // 获取页表项
    pte = walk(p->pagetable, va, 0);
    if (pte == 0)
        return -1;
    if ((*pte & PTE_V) == 0)
        return -1;
    if ((*pte & PTE_COW) == 0)
        return -1;  // 不是COW页面

    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);

    // 获取引用计数
    int ref = get_ref(pa);

    if (ref > 1) {
        // 多个进程共享，需要复制
        new_page = kalloc();
        if (new_page == 0) {
            // 内存不足
            return -1;
        }

        // 复制数据
        memmove(new_page, (char *)pa, PGSIZE);

        // 减少原页面引用计数
        decrement_ref(pa);

        // 更新页表指向新页面
        flags = (flags & ~PTE_COW) | PTE_W;  // 清除COW，设置可写
        *pte = PA2PTE((uint64)new_page) | flags;
    } else {
        // 只有一个引用，直接设置为可写
        flags = (flags & ~PTE_COW) | PTE_W;
        *pte = PA2PTE(pa) | flags;
    }

    return 0;
}
