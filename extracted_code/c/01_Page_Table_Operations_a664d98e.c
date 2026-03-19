/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 1451
 * Language: c
 * Block ID: a664d98e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 大页管理 - 透明大页(THP)和显式大页(Hugetlb)
 */

# include <linux/huge_mm.h>
# include <linux/mm.h>

/*
 * 检查是否可以使用透明大页
 */
static inline bool __transparent_hugepage_enabled(struct vm_area_struct *vma)
{
    /* 检查系统全局设置 */
    if (!test_bit(TRANSPARENT_HUGEPAGE_FLAG, &transparent_hugepage_flags))
        return false;

    /* 检查VMA标志 */
    if (vma->vm_flags & VM_NOHUGEPAGE)
        return false;

    /* 匿名映射 */
    if (vma_is_anonymous(vma))
        return true;

    /* shmem/tmpfs映射 */
    if (shmem_mapping(vma->vm_file))
        return shmem_huge_enabled(vma);

    return false;
}

/*
 * 尝试将PMD条目转换为透明大页
 * 由khugepaged守护进程调用
 */
int collapse_huge_page(struct mm_struct *mm, unsigned long addr,
                       struct page **hpage, struct vm_area_struct *vma,
                       int node)
{
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte, *_pte;
    spinlock_t *ptl;
    int isolated = 0;
    unsigned long haddr = addr & HPAGE_PMD_MASK;
    int i, result = -EBUSY;

    /* 获取页表锁 */
    pgd = pgd_offset(mm, haddr);
    p4d = p4d_offset(pgd, haddr);
    pud = pud_offset(p4d, haddr);
    pmd = pmd_offset(pud, haddr);

    /* 检查PMD是否为空 */
    ptl = pmd_lock(mm, pmd);
    if (!pmd_none(*pmd)) {
        spin_unlock(ptl);
        return -EBUSY;
    }
    spin_unlock(ptl);

    /* 检查PTE页是否全部存在且可合并 */
    pte = pte_offset_map_lock(mm, pmd, haddr, &ptl);

    for (i = 0, _pte = pte; i < HPAGE_PMD_NR; i++, _pte++) {
        pte_t pteval = *_pte;

        if (!pte_present(pteval) || !pte_write(pteval)) {
            pte_unmap_unlock(pte, ptl);
            return -EBUSY;
        }

        /* 检查页是否可以隔离 */
        struct page *page = pfn_to_page(pte_pfn(pteval));
        if (!PageLRU(page) || !get_page_unless_zero(page)) {
            pte_unmap_unlock(pte, ptl);
            return -EBUSY;
        }

        isolated++;
    }
    pte_unmap_unlock(pte, ptl);

    /* 分配大页 */
    if (!*hpage) {
        *hpage = alloc_hugepage_vma(GFP_TRANSHUGE, vma, haddr, node);
        if (!*hpage) {
            result = -ENOMEM;
            goto out_isolate;
        }
    }

    /* 复制数据到大页 */
    pte = pte_offset_map_lock(mm, pmd, haddr, &ptl);

    for (i = 0, _pte = pte; i < HPAGE_PMD_NR; i++, _pte++) {
        pte_t pteval = *_pte;
        struct page *page = pfn_to_page(pte_pfn(pteval));
        void *src = kmap_atomic(page);
        void *dst = kmap_atomic(*hpage + i);

        copy_page(dst, src);

        kunmap_atomic(dst);
        kunmap_atomic(src);
    }

    /* 释放小页 */
    for (i = 0, _pte = pte; i < HPAGE_PMD_NR; i++, _pte++) {
        pte_t pteval = *_pte;
        struct page *page = pfn_to_page(pte_pfn(pteval));

        page_remove_rmap(page);
        put_page(page);
    }

    /* 设置PMD大页条目 */
    pmd_t _pmd = mk_huge_pmd(*hpage, vma->vm_page_prot);
    _pmd = pmd_mkyoung(_pmd);
    _pmd = pmd_mkdirty(_pmd);
    _pmd = pmd_mkwrite(_pmd);

    set_pmd_at(mm, haddr, pmd, _pmd);
    update_mmu_cache_pmd(vma, haddr, pmd);

    pte_unmap_unlock(pte, ptl);

    /* 释放PTE页表 */
    pte_free(mm, (pte_t *)pmd_pgtable(*pmd));
    mm_dec_nr_ptes(mm);

    result = 0;

out_isolate:
    /* 如果失败，恢复隔离的页 */
    if (result) {
        pte = pte_offset_map_lock(mm, pmd, haddr, &ptl);
        for (i = 0, _pte = pte; i < isolated; i++, _pte++) {
            pte_t pteval = *_pte;
            struct page *page = pfn_to_page(pte_pfn(pteval));
            put_page(page);
        }
        pte_unmap_unlock(pte, ptl);
    }

    return result;
}

/*
 * 拆分透明大页为普通页
 * 当需要部分修改大页时调用
 */
int split_huge_page_to_list(struct page *page, struct list_head *list)
{
    struct page *head = compound_head(page);
    struct anon_vma *anon_vma;
    struct address_space *mapping;
    struct mm_struct *mm;
    pgoff_t pgoff;
    pmd_t *pmd;
    int ret = 0;

    /* 获取大页的anon_vma */
    anon_vma = page_lock_anon_vma_read(head);
    if (!anon_vma)
        return -EBUSY;

    /* 遍历所有映射此页的VMA */
    anon_vma_interval_tree_foreach(mapping, &anon_vma->rb_root, 0, ULONG_MAX) {
        struct anon_vma_chain *avc;

        list_for_each_entry(avc, &mapping->same_vma, same_vma) {
            struct vm_area_struct *vma = avc->vma;
            unsigned long addr;

            addr = vma->vm_start + ((pgoff - vma->vm_pgoff) << PAGE_SHIFT);

            /* 获取PMD */
            pmd = mm_find_pmd(vma->vm_mm, addr);
            if (!pmd)
                continue;

            /* 拆分PMD */
            ret = split_huge_pmd(vma, pmd, addr);
            if (ret)
                goto out;
        }
    }

out:
    page_unlock_anon_vma_read(anon_vma);
    return ret;
}

/*
 * Hugetlbfs显式大页分配
 */
struct page *alloc_huge_page(struct vm_area_struct *vma,
                             unsigned long addr, int avoid_reserve)
{
    struct hstate *h = hstate_vma(vma);
    struct page *page;

    /* 检查保留页 */
    if (!avoid_reserve && vma_has_reserves(vma, addr)) {
        page = dequeue_huge_page_vma(h, vma, addr, avoid_reserve);
        if (page)
            return page;
    }

    /* 从全局池分配 */
    spin_lock(&hugetlb_lock);
    page = dequeue_huge_page_nodemask(h, cpuset_mems_allowed(current));
    spin_unlock(&hugetlb_lock);

    if (!page) {
        /* 尝试从伙伴系统分配 */
        page = alloc_fresh_huge_page(h, nid);
    }

    return page;
}
