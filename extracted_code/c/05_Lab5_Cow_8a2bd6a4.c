/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\05_Lab5_Cow.md
 * Line: 222
 * Language: c
 * Block ID: 8a2bd6a4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/vm.c

// 复制页表，但不复制物理页面，使用COW
int
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
    pte_t *pte;
    uint64 pa, i;
    uint flags;

    for (i = 0; i < sz; i += PGSIZE) {
        if ((pte = walk(old, i, 0)) == 0)
            panic("uvmcopy: pte should exist");
        if ((*pte & PTE_V) == 0)
            panic("uvmcopy: page not present");

        pa = PTE2PA(*pte);
        flags = PTE_FLAGS(*pte);

        // 如果是可写页面，转换为COW
        if (flags & PTE_W) {
            // 清除写权限，添加COW标志
            flags = (flags & ~PTE_W) | PTE_COW;
            *pte = PA2PTE(pa) | flags;  // 更新父进程页表
        }

        // 映射到子进程 (共享同一物理页)
        if (mappages(new, i, PGSIZE, pa, flags) != 0) {
            // 失败，取消映射已映射的页面
            uvmunmap(new, 0, i / PGSIZE, 0);
            return -1;
        }

        // 增加引用计数
        increment_ref(pa);
    }

    return 0;
}
