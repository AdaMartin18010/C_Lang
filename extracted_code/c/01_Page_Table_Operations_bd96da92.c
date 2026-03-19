/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 620
 * Language: c
 * Block ID: bd96da92
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 页表遍历实现 - 模拟Linux内核逻辑
 * 适用于x86-64 4级页表
 */

# include <linux/mm.h>
# include <linux/pgtable.h>
# include <asm/pgalloc.h>

/*
 * 完整页表遍历 - 从PGD到PTE
 * 返回PTE指针，如果中间级不存在则返回NULL
 */
pte_t *walk_page_table(struct mm_struct *mm, unsigned long addr)
{
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    /* Level 1: PGD (Page Global Directory) */
    pgd = pgd_offset(mm, addr);
    if (pgd_none(*pgd) || pgd_bad(*pgd)) {
        pr_debug("PGD not present for addr %lx\n", addr);
        return NULL;
    }

    /* Level 2: P4D (Page 4th Directory) - 兼容5级页表 */
    p4d = p4d_offset(pgd, addr);
    if (p4d_none(*p4d) || p4d_bad(*p4d)) {
        pr_debug("P4D not present for addr %lx\n", addr);
        return NULL;
    }

    /* Level 3: PUD (Page Upper Directory) */
    pud = pud_offset(p4d, addr);
    if (pud_none(*pud) || pud_bad(*pud)) {
        pr_debug("PUD not present for addr %lx\n", addr);
        return NULL;
    }

    /* Level 4: PMD (Page Middle Directory) */
    pmd = pmd_offset(pud, addr);
    if (pmd_none(*pmd) || pmd_bad(*pmd)) {
        pr_debug("PMD not present for addr %lx\n", addr);
        return NULL;
    }

    /* 检查透明大页 - PMD级大页 */
    if (pmd_trans_huge(*pmd)) {
        pr_debug("Transparent huge page at PMD for addr %lx\n", addr);
        /* 返回PMD的别名作为"大PTE" */
        return (pte_t *)pmd;
    }

    /* Level 5: PTE (Page Table Entry) */
    pte = pte_offset_map(pmd, addr);

    return pte;
}

/*
 * 带锁的页表遍历 - 线程安全版本
 */
pte_t *walk_page_table_locked(struct mm_struct *mm, unsigned long addr,
                               spinlock_t **ptlp)
{
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    pgd = pgd_offset(mm, addr);
    if (pgd_none(*pgd))
        return NULL;

    p4d = p4d_offset(pgd, addr);
    if (p4d_none(*p4d))
        return NULL;

    pud = pud_offset(p4d, addr);
    if (pud_none(*pud))
        return NULL;

    pmd = pmd_offset(pud, addr);
    if (pmd_none(*pmd))
        return NULL;

    /* 使用pte_offset_map_lock获取PTE并加锁 */
    pte = pte_offset_map_lock(mm, pmd, addr, ptlp);

    return pte;
}
