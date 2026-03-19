/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 800
 * Language: c
 * Block ID: 2c2563ca
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 页分配与释放 - 模拟伙伴系统核心逻辑
 */

# include <linux/mm.h>
# include <linux/gfp.h>
# include <linux/page-flags.h>
# include <asm/pgalloc.h>

/*
 * 分配页表页（4KB对齐）
 * 实际使用alloc_page()或__get_free_page()
 */
void *alloc_page_table(struct mm_struct *mm)
{
    struct page *page;
    void *table;

    /* GFP_KERNEL: 可能睡眠，用于进程上下文 */
    /* __GFP_ZERO: 清零分配的页 */
    page = alloc_page(GFP_KERNEL | __GFP_ZERO);
    if (!page)
        return NULL;

    /* 增加页表页计数 */
    if (mm)
        mm->pgtables_bytes += PAGE_SIZE;

    /* 返回虚拟地址 */
    table = page_address(page);

    /* 确保页表项已清零（安全考虑） */
    memset(table, 0, PAGE_SIZE);

    return table;
}

/*
 * 释放页表页
 */
void free_page_table(struct mm_struct *mm, void *table)
{
    struct page *page;

    if (!table)
        return;

    page = virt_to_page(table);

    /* 减少页表页计数 */
    if (mm)
        mm->pgtables_bytes -= PAGE_SIZE;

    /* 释放页 */
    __free_page(page);
}

/*
 * 分配并初始化PTE页表
 */
pte_t *pte_alloc_one(struct mm_struct *mm, unsigned long addr)
{
    pte_t *pte;

    pte = (pte_t *)alloc_page_table(mm);
    if (!pte)
        return NULL;

    /* 初始化页表条目为无效 */
    /* 已在alloc_page_table中通过__GFP_ZERO清零 */

    return pte;
}

/*
 * 批量释放页表（递归释放）
 * 这是munmap的核心逻辑
 */
void free_pte_range(struct mm_struct *mm, pmd_t *pmd,
                    unsigned long addr, unsigned long end)
{
    pte_t *pte, *pte_base;
    unsigned long pfn;

    pte_base = pte_offset_map(pmd, addr);
    pte = pte_base;

    for (; addr < end; addr += PAGE_SIZE, pte++) {
        if (!pte_present(*pte))
            continue;

        pfn = pte_pfn(*pte);
        if (pfn_valid(pfn)) {
            struct page *page = pfn_to_page(pfn);

            /* 减少引用计数 */
            if (page_mapcount(page) > 0)
                page_mapcount_reset(page);

            /* 释放页 */
            if (!PageReserved(page))
                __free_page(page);
        }

        /* 清除PTE */
        pte_clear(mm, addr, pte);
    }

    pte_unmap(pte_base);

    /* 释放PTE页表本身 */
    pmd_clear(pmd);
    free_page_table(mm, pte_base);
}
