/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 1203
 * Language: c
 * Block ID: 0dafa610
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 页错误处理 - 完整实现模拟
 */

# include <linux/mm.h>
# include <linux/signal.h>
# include <asm/traps.h>

/* 错误码定义 */
# define PF_PROT     (1 << 0)   /* 保护错误（权限不足） */
# define PF_WRITE    (1 << 1)   /* 写访问 */
# define PF_USER     (1 << 2)   /* 用户态访问 */
# define PF_RSVD     (1 << 3)   /* 保留位错误 */
# define PF_INSTR    (1 << 4)   /* 取指错误 */

/*
 * 主页错误处理函数
 * 在x86上由do_page_fault汇编入口调用
 */
void do_page_fault(struct pt_regs *regs, unsigned long error_code)
{
    struct task_struct *tsk = current;
    struct mm_struct *mm = tsk->mm;
    unsigned long address = read_cr2();  /* 获取故障地址 */
    struct vm_area_struct *vma;
    int si_code = SEGV_MAPERR;
    int fault;
    unsigned int flags = FAULT_FLAG_DEFAULT;

    /* 标记不可重入 */
    if (pagefault_disabled())
        goto no_context;

    /* 解析错误码 */
    if (error_code & PF_WRITE)
        flags |= FAULT_FLAG_WRITE;
    if (error_code & PF_USER)
        flags |= FAULT_FLAG_USER;
    if (!(error_code & PF_PROT))
        flags |= FAULT_FLAG_INSTRUCTION;

    /* 内核态页错误处理 */
    if (!user_mode(regs)) {
        /* 检查是否是vmalloc区域 */
        if (address >= TASK_SIZE_MAX) {
            if (vmalloc_fault(address) >= 0)
                return;
        }

        /* 内核访问无效地址 */
        if (!mm)
            goto no_context;
    }

    /* 查找VMA */
    vma = find_vma(mm, address);
    if (!vma)
        goto bad_area;

    if (vma->vm_start <= address)
        goto good_area;

    /* 检查栈增长 */
    if (!(vma->vm_flags & VM_GROWSDOWN))
        goto bad_area;

    if (error_code & PF_USER) {
        if (expand_stack(vma, address))
            goto bad_area;
    }

good_area:
    si_code = SEGV_ACCERR;

    /* 权限检查 */
    if (error_code & PF_WRITE) {
        if (!(vma->vm_flags & VM_WRITE))
            goto bad_area;
    } else {
        if (error_code & PF_INSTR) {
            if (!(vma->vm_flags & VM_EXEC))
                goto bad_area;
        }
        if (!(vma->vm_flags & (VM_READ | VM_EXEC)))
            goto bad_area;
    }

    /* 处理缺页 */
    fault = handle_mm_fault(vma, address, flags);

    /* 检查处理结果 */
    if (unlikely(fault & VM_FAULT_ERROR)) {
        if (fault & VM_FAULT_OOM)
            goto out_of_memory;
        if (fault & VM_FAULT_SIGBUS)
            goto do_sigbus;
        BUG();
    }

    return;

bad_area:
    /* 用户态访问无效地址 */
    if (user_mode(regs)) {
        force_sig_fault(SIGSEGV, si_code, (void __user *)address);
        return;
    }

no_context:
    /* 内核态访问无效地址 - 内核bug */
    if (fixup_exception(regs))
        return;

    /* 致命错误 */
    die("Oops", regs, error_code);

out_of_memory:
    /* 内存不足 */
    if (!user_mode(regs))
        goto no_context;
    pagefault_out_of_memory();
    return;

do_sigbus:
    force_sig_fault(SIGBUS, BUS_ADRERR, (void __user *)address);
}

/*
 * 匿名页缺页处理
 */
int do_anonymous_page(struct vm_area_struct *vma, unsigned long address,
                      pte_t *page_table, pmd_t *pmd, unsigned int flags)
{
    struct mm_struct *mm = vma->vm_mm;
    struct page *page;
    spinlock_t *ptl;
    pte_t entry;

    /* 检查是否为纯零页（延迟分配） */
    if (!(flags & FAULT_FLAG_WRITE) && !vma_needs_reservation(vma, address)) {
        /* 使用零页（全局只读页） */
        entry = pte_mkspecial(pfn_pte(my_zero_pfn(address), vma->vm_page_prot));

        ptl = pte_lockptr(mm, pmd);
        spin_lock(ptl);

        if (!pte_none(*page_table))
            goto unlock;

        set_pte_at(mm, address, page_table, entry);
        update_mmu_cache(vma, address, page_table);

unlock:
        spin_unlock(ptl);
        return 0;
    }

    /* 分配新页 */
    page = alloc_zeroed_user_highpage_movable(vma, address);
    if (!page)
        return VM_FAULT_OOM;

    /* 设置页属性 */
    if (flags & FAULT_FLAG_WRITE) {
        entry = pte_mkwrite(pte_mkdirty(mk_pte(page, vma->vm_page_prot)));
    } else {
        entry = mk_pte(page, vma->vm_page_prot);
    }

    /* 锁定并设置PTE */
    ptl = pte_lockptr(mm, pmd);
    spin_lock(ptl);

    if (!pte_none(*page_table)) {
        /* 并发设置，释放我们分配的页 */
        spin_unlock(ptl);
        put_page(page);
        return 0;
    }

    /* 增加RSS计数 */
    inc_mm_counter(mm, MM_ANONPAGES);
    page_add_new_anon_rmap(page, vma, address);

    set_pte_at(mm, address, page_table, entry);
    update_mmu_cache(vma, address, page_table);

    spin_unlock(ptl);
    return 0;
}

/*
 * 写时复制（COW）处理
 */
int do_wp_page(struct vm_area_struct *vma, unsigned long address,
               pte_t *page_table, pmd_t *pmd, spinlock_t *ptl, pte_t orig_pte)
{
    struct page *old_page, *new_page;
    pte_t entry;

    old_page = vm_normal_page(vma, address, orig_pte);
    if (!old_page)
        goto reuse;

    /* 检查页引用计数 */
    if (page_count(old_page) == 1) {
        /* 只有一个引用，可以直接重用 */
        reuse:
        entry = pte_mkyoung(orig_pte);
        entry = maybe_mkwrite(pte_mkdirty(entry), vma);

        if (ptep_set_access_flags(vma, address, page_table, entry, 1)) {
            update_mmu_cache(vma, address, page_table);
        }
        pte_unmap_unlock(page_table, ptl);
        return 0;
    }

    /* 需要复制页 */
    new_page = alloc_page_vma(GFP_HIGHUSER_MOVABLE, vma, address);
    if (!new_page) {
        pte_unmap_unlock(page_table, ptl);
        return VM_FAULT_OOM;
    }

    /* 复制旧页内容 */
    copy_user_highpage(new_page, old_page, address, vma);

    /* 设置新的PTE */
    entry = mk_pte(new_page, vma->vm_page_prot);
    entry = pte_mkwrite(pte_mkdirty(entry));
    entry = pte_mkyoung(entry);

    set_pte_at(vma->vm_mm, address, page_table, entry);
    update_mmu_cache(vma, address, page_table);

    /* 减少旧页引用 */
    page_remove_rmap(old_page);
    put_page(old_page);

    pte_unmap_unlock(page_table, ptl);
    return 0;
}
