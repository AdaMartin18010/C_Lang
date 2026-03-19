/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 1988
 * Language: c
 * Block ID: f7e0c050
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ❌ 错误：分配页表后未释放
 * 导致内存泄漏
 */
void buggy_create_mapping(struct mm_struct *mm, unsigned long addr)
{
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    pgd = pgd_offset(mm, addr);

    if (pgd_none(*pgd)) {
        pud = (pud_t *)get_zeroed_page(GFP_KERNEL);
        if (!pud)
            return;
        set_pgd(pgd, __pgd(_PAGE_TABLE | __pa(pud)));
    }

    /* 中间某些路径可能直接返回，导致已分配的pud未使用 */
    if (some_error_condition())
        return;  // pud泄漏！

    pud = pud_offset(pgd, addr);
    if (pud_none(*pud)) {
        pmd = (pmd_t *)get_zeroed_page(GFP_KERNEL);
        if (!pmd)
            return;  // 应该回滚pud的分配
        set_pud(pud, __pud(_PAGE_TABLE | __pa(pmd)));
    }

    /* ... */
}

/*
 * ✅ 正确：使用内核提供的分配函数，正确处理错误路径
 */
int correct_create_mapping(struct mm_struct *mm, unsigned long addr)
{
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;

    pgd = pgd_offset(mm, addr);

    /* 使用内核提供的分配函数，自动处理错误 */
    p4d = p4d_alloc(mm, pgd, addr);
    if (!p4d)
        return -ENOMEM;

    pud = pud_alloc(mm, p4d, addr);
    if (!pud)
        return -ENOMEM;

    pmd = pmd_alloc(mm, pud, addr);
    if (!pmd)
        return -ENOMEM;

    /* ... */
    return 0;
}

/*
 * munmap时未释放中间页表
 */
void buggy_unmap_range(struct mm_struct *mm, unsigned long start,
                       unsigned long end)
{
    for (; start < end; start += PAGE_SIZE) {
        pte_t *pte = walk_page_table(mm, start);
        if (pte && pte_present(*pte)) {
            pte_clear(mm, start, pte);
        }
    }
    /* 错误：未释放PTE、PMD、PUD页表本身！ */
}
