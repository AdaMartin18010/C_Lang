/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 1941
 * Language: c
 * Block ID: 2f8beb24
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ❌ 错误：无锁并发访问页表
 * 可能导致数据损坏
 */
void buggy_modify_pte(struct mm_struct *mm, unsigned long addr)
{
    pte_t *pte = walk_page_table(mm, addr);

    /* 无锁访问 - 竞态条件！ */
    if (pte_write(*pte)) {
        /* 另一个CPU可能同时修改PTE */
        pte_t new_pte = pte_wrprotect(*pte);
        set_pte(pte, new_pte);  // 可能覆盖其他修改
    }
}

/*
 * ✅ 正确：使用pte_offset_map_lock保护
 */
void correct_modify_pte(struct mm_struct *mm, unsigned long addr)
{
    pmd_t *pmd = walk_to_pmd(mm, addr);
    pte_t *pte;
    spinlock_t *ptl;
    pte_t entry, old_entry;

    /* 获取PTE锁 */
    pte = pte_offset_map_lock(mm, pmd, addr, &ptl);

    /* 安全的检查和修改 */
    old_entry = *pte;
    if (pte_write(old_entry)) {
        entry = pte_wrprotect(old_entry);
        set_pte_at(mm, addr, pte, entry);

        /* 刷新TLB */
        flush_tlb_page(vma, addr);
    }

    /* 释放锁 */
    pte_unmap_unlock(pte, ptl);
}
