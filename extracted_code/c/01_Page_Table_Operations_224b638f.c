/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 1684
 * Language: c
 * Block ID: 224b638f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ❌ 错误：未检查页表条目有效性
 * 可能导致访问无效内存或内核panic
 */
pte_t *buggy_walk_page_table(struct mm_struct *mm, unsigned long addr)
{
    pgd_t *pgd = pgd_offset(mm, addr);
    p4d_t *p4d = p4d_offset(pgd, addr);  // 危险：未检查pgd是否有效
    pud_t *pud = pud_offset(p4d, addr);  // 危险：未检查p4d是否有效
    pmd_t *pmd = pmd_offset(pud, addr);  // 危险：未检查pud是否有效
    pte_t *pte = pte_offset_map(pmd, addr);  // 危险：未检查pmd是否有效

    return pte;  // 可能返回无效指针
}

/*
 * ✅ 正确：逐级检查页表条目有效性
 */
pte_t *correct_walk_page_table(struct mm_struct *mm, unsigned long addr)
{
    pgd_t *pgd = pgd_offset(mm, addr);
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    if (pgd_none(*pgd) || pgd_bad(*pgd))
        return NULL;

    p4d = p4d_offset(pgd, addr);
    if (p4d_none(*p4d) || p4d_bad(*p4d))
        return NULL;

    pud = pud_offset(p4d, addr);
    if (pud_none(*pud) || pud_bad(*pud))
        return NULL;

    pmd = pmd_offset(pud, addr);
    if (pmd_none(*pmd) || pmd_bad(*pmd))
        return NULL;

    pte = pte_offset_map(pmd, addr);
    return pte;
}
