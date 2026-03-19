/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 1836
 * Language: c
 * Block ID: fae55e96
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ❌ 错误：大页映射时未对齐
 * 导致硬件异常或数据损坏
 */
int buggy_map_huge_page(struct mm_struct *mm, unsigned long addr,
                        phys_addr_t phys, pgprot_t prot)
{
    pmd_t *pmd;

    /* 错误：未检查地址是否2MB对齐 */
    pmd = pmd_alloc(mm, pud, addr);

    /* 错误：未检查物理地址对齐 */
    set_pmd_at(mm, addr, pmd, pfn_pmd(phys >> PAGE_SHIFT, prot));

    return 0;
}

/*
 * ✅ 正确：检查大页对齐
 */
int correct_map_huge_page(struct mm_struct *mm, unsigned long addr,
                          phys_addr_t phys, pgprot_t prot)
{
    pmd_t *pmd;

    /* 1. 检查虚拟地址2MB对齐 */
    if (!IS_ALIGNED(addr, PMD_SIZE)) {
        pr_err("Virtual address %lx not aligned to PMD_SIZE\n", addr);
        return -EINVAL;
    }

    /* 2. 检查物理地址2MB对齐 */
    if (!IS_ALIGNED(phys, PMD_SIZE)) {
        pr_err("Physical address %pa not aligned to PMD_SIZE\n", &phys);
        return -EINVAL;
    }

    pmd = pmd_alloc(mm, pud, addr);
    if (!pmd)
        return -ENOMEM;

    /* 3. 检查PMD是否已使用 */
    if (!pmd_none(*pmd)) {
        pr_err("PMD already in use at %lx\n", addr);
        return -EBUSY;
    }

    /* 4. 设置大页PMD条目 */
    set_pmd_at(mm, addr, pmd,
               pmd_mkhuge(pfn_pmd(phys >> PAGE_SHIFT, prot)));

    /* 5. 刷新TLB */
    flush_tlb_range(vma, addr, addr + PMD_SIZE);

    return 0;
}
