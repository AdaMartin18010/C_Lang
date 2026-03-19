/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\05_Lab5_Cow.md
 * Line: 383
 * Language: c
 * Block ID: 9004d591
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/vm.c

int
copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len)
{
    uint64 n, va0, pa0;
    pte_t *pte;

    while (len > 0) {
        va0 = PGROUNDDOWN(dstva);

        // 检查是否需要处理COW
        pte = walk(pagetable, va0, 0);
        if (pte && (*pte & PTE_V) && (*pte & PTE_COW)) {
            // 需要复制COW页面
            uint64 pa = PTE2PA(*pte);
            int ref = get_ref(pa);

            if (ref > 1) {
                char *new_page = kalloc();
                if (new_page == 0)
                    return -1;

                memmove(new_page, (char *)pa, PGSIZE);
                decrement_ref(pa);

                uint flags = (PTE_FLAGS(*pte) & ~PTE_COW) | PTE_W;
                *pte = PA2PTE((uint64)new_page) | flags;
                pa0 = (uint64)new_page;
            } else {
                uint flags = (PTE_FLAGS(*pte) & ~PTE_COW) | PTE_W;
                *pte = PA2PTE(pa) | flags;
                pa0 = pa;
            }
        } else {
            pa0 = walkaddr(pagetable, va0);
            if (pa0 == 0)
                return -1;
        }

        n = PGSIZE - (dstva - va0);
        if (n > len)
            n = len;

        memmove((void *)(pa0 + (dstva - va0)), src, n);

        len -= n;
        src += n;
        dstva = va0 + PGSIZE;
    }

    return 0;
}
