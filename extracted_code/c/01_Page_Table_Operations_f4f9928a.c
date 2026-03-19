/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 1733
 * Language: c
 * Block ID: f4f9928a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ❌ 错误：修改页表后未刷新TLB
 * 导致CPU可能使用旧的地址映射
 */
void buggy_unmap_page(struct mm_struct *mm, unsigned long addr)
{
    pte_t *pte = walk_page_table(mm, addr);
    if (pte) {
        pte_clear(mm, addr, pte);  // 清除PTE
        // 错误：没有刷新TLB！
    }
}

/*
 * ✅ 正确：修改页表后必须刷新TLB
 */
void correct_unmap_page(struct mm_struct *mm, unsigned long addr)
{
    pte_t *pte = walk_page_table(mm, addr);
    spinlock_t *ptl;

    if (pte) {
        ptep = pte_offset_map_lock(mm, pmd, addr, &ptl);

        /* 1. 清除PTE */
        pte_clear(mm, addr, ptep);

        /* 2. 刷新TLB */
        flush_tlb_page(vma, addr);

        pte_unmap_unlock(ptep, ptl);
    }
}

/*
 * ❌ 错误：在持有自旋锁时调用可能睡眠的TLB刷新
 */
void buggy_flush_with_lock(struct mm_struct *mm, unsigned long addr)
{
    spinlock_t *ptl;
    pte_t *pte;

    pte = pte_offset_map_lock(mm, pmd, addr, &ptl);

    /* 某些TLB刷新操作可能睡眠！ */
    tlb_flush_mmu(&tlb);  // 危险：可能睡眠

    pte_unmap_unlock(pte, ptl);
}
