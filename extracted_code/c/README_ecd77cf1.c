/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\README.md
 * Line: 987
 * Language: c
 * Block ID: ecd77cf1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 页表遍历与操作示例
 * 适用于x86-64架构
 */

#include <linux/mm.h>
#include <linux/pgtable.h>
#include <asm/page.h>
#include <asm/pgalloc.h>

/* x86-64四级页表结构 */
#define PML4_SHIFT  39
#define PDPT_SHIFT  30
#define PD_SHIFT    21
#define PT_SHIFT    12

#define PTRS_PER_PML4   512
#define PTRS_PER_PDPT   512
#define PTRS_PER_PD     512
#define PTRS_PER_PT     512

/* 页表项提取宏 */
#define pml4_index(addr)    (((addr) >> PML4_SHIFT) & (PTRS_PER_PML4 - 1))
#define pdpt_index(addr)    (((addr) >> PDPT_SHIFT) & (PTRS_PER_PDPT - 1))
#define pd_index(addr)      (((addr) >> PD_SHIFT) & (PTRS_PER_PD - 1))
#define pt_index(addr)      (((addr) >> PT_SHIFT) & (PTRS_PER_PT - 1))

/* 遍历页表获取物理地址 */
phys_addr_t virt_to_phys_custom(struct mm_struct *mm, unsigned long vaddr)
{
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    phys_addr_t paddr;

    /* 获取PGD（Page Global Directory） */
    pgd = pgd_offset(mm, vaddr);
    if (pgd_none(*pgd) || pgd_bad(*pgd))
        return 0;

    /* P4D（Page 4th Directory）- 五级页表使用 */
    p4d = p4d_offset(pgd, vaddr);
    if (p4d_none(*p4d) || p4d_bad(*p4d))
        return 0;

    /* PUD（Page Upper Directory）/ PDPT */
    pud = pud_offset(p4d, vaddr);
    if (pud_none(*pud) || pud_bad(*pud))
        return 0;

    /* 检查是否为1GB大页 */
    if (pud_large(*pud)) {
        paddr = (phys_addr_t)pud_pfn(*pud) << PAGE_SHIFT;
        paddr |= vaddr & ~PUD_MASK;
        return paddr;
    }

    /* PMD（Page Middle Directory） */
    pmd = pmd_offset(pud, vaddr);
    if (pmd_none(*pmd) || pmd_bad(*pmd))
        return 0;

    /* 检查是否为2MB大页 */
    if (pmd_large(*pmd)) {
        paddr = (phys_addr_t)pmd_pfn(*pmd) << PAGE_SHIFT;
        paddr |= vaddr & ~PMD_MASK;
        return paddr;
    }

    /* PTE（Page Table Entry） */
    pte = pte_offset_map(pmd, vaddr);
    if (!pte_present(*pte)) {
        pte_unmap(pte);
        return 0;
    }

    paddr = (phys_addr_t)pte_pfn(*pte) << PAGE_SHIFT;
    paddr |= vaddr & ~PAGE_MASK;

    pte_unmap(pte);
    return paddr;
}

/* 修改页表项权限 */
int set_page_protection(struct mm_struct *mm, unsigned long vaddr,
                        pgprot_t new_prot)
{
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte, old_pte;
    spinlock_t *ptl;
    int ret = 0;

    /* 获取页表锁 */
    pgd = pgd_offset(mm, vaddr);
    if (pgd_none(*pgd))
        return -EINVAL;

    p4d = p4d_offset(pgd, vaddr);
    if (p4d_none(*p4d))
        return -EINVAL;

    pud = pud_offset(p4d, vaddr);
    if (pud_none(*pud))
        return -EINVAL;

    pmd = pmd_offset(pud, vaddr);
    if (pmd_none(*pmd))
        return -EINVAL;

    /* 获取PTE并锁定 */
    pte = pte_offset_map_lock(mm, pmd, vaddr, &ptl);
    if (!pte_present(*pte)) {
        ret = -EINVAL;
        goto unlock;
    }

    old_pte = *pte;

    /* 修改保护位 */
    set_pte_at(mm, vaddr, pte, pte_modify(old_pte, new_prot));

    /* 刷新TLB */
    flush_tlb_page(vma, vaddr);

unlock:
    pte_unmap_unlock(pte, ptl);
    return ret;
}

/* 分配并映射物理连续内存 */
void *alloc_contiguous_pages(struct mm_struct *mm, unsigned long vaddr,
                             size_t num_pages, pgprot_t prot)
{
    struct page *pages;
    unsigned long pfn;
    int i;

    /* 分配物理连续页 */
    pages = alloc_pages(GFP_KERNEL | __GFP_COMP, get_order(num_pages * PAGE_SIZE));
    if (!pages)
        return NULL;

    pfn = page_to_pfn(pages);

    /* 建立映射 */
    for (i = 0; i < num_pages; i++) {
        int ret = vm_insert_pfn(
            find_vma(mm, vaddr + i * PAGE_SIZE),
            vaddr + i * PAGE_SIZE,
            pfn + i
        );
        if (ret) {
            /* 清理已建立的映射 */
            while (--i >= 0) {
                zap_page_range(find_vma(mm, vaddr),
                               vaddr + i * PAGE_SIZE, PAGE_SIZE);
            }
            __free_pages(pages, get_order(num_pages * PAGE_SIZE));
            return NULL;
        }
    }

    return (void *)vaddr;
}
