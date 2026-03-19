/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 720
 * Language: c
 * Block ID: b0c6bb88
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 虚拟地址到物理地址转换
 * 模拟内核中的vtop逻辑
 */

# include <linux/mm.h>
# include <asm/page.h>

/*
 * 用户态虚拟地址转物理地址
 * 注意：此函数仅用于调试，实际内核中有专门的API
 */
phys_addr_t virt_to_phys_user(struct mm_struct *mm, unsigned long vaddr)
{
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    phys_addr_t paddr = 0;
    unsigned long pfn;

    pgd = pgd_offset(mm, vaddr);
    if (pgd_none(*pgd) || pgd_bad(*pgd))
        return 0;

    p4d = p4d_offset(pgd, vaddr);
    if (p4d_none(*p4d) || p4d_bad(*p4d))
        return 0;

    pud = pud_offset(p4d, vaddr);
    if (pud_none(*pud) || pud_bad(*pud))
        return 0;

    pmd = pmd_offset(pud, vaddr);
    if (pmd_none(*pmd) || pmd_bad(*pmd))
        return 0;

    /* 处理2MB大页 */
    if (pmd_large(*pmd)) {
        pfn = pmd_pfn(*pmd);
        paddr = (pfn << PAGE_SHIFT) | (vaddr & ~PMD_MASK);
        return paddr;
    }

    pte = pte_offset_map(pmd, vaddr);
    if (!pte_present(*pte)) {
        pte_unmap(pte);
        return 0;
    }

    pfn = pte_pfn(*pte);
    paddr = (pfn << PAGE_SHIFT) | (vaddr & ~PAGE_MASK);

    pte_unmap(pte);
    return paddr;
}

/*
 * 内核虚拟地址转物理地址（简化版）
 * 实际使用__pa()宏
 */
static inline phys_addr_t __pa_kernel(void *vaddr)
{
    /* 线性映射区域: phys = virt - PAGE_OFFSET */
    return (phys_addr_t)(vaddr) - PAGE_OFFSET;
}

/*
 * 物理地址转内核虚拟地址
 */
static inline void *__va_kernel(phys_addr_t paddr)
{
    return (void *)(paddr + PAGE_OFFSET);
}
