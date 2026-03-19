/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 1787
 * Language: c
 * Block ID: d624bfa0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ❌ 错误：在缺页处理中递归获取页表锁
 * 可能导致死锁
 */
int buggy_handle_fault(struct vm_area_struct *vma, unsigned long addr)
{
    pgd_t *pgd;
    spinlock_t *ptl;

    /* 获取高层锁 */
    pgd = pgd_offset(vma->vm_mm, addr);
    spin_lock(&vma->vm_mm->page_table_lock);

    /* 分配下级页表 - 可能触发缺页！ */
    pud = pud_alloc(vma->vm_mm, p4d, addr);  // 危险：可能触发递归缺页

    /* 尝试再次获取锁 - 死锁！ */
    spin_lock(&vma->vm_mm->page_table_lock);  // 死锁

    spin_unlock(&vma->vm_mm->page_table_lock);
    return 0;
}

/*
 * ✅ 正确：使用适当的锁粒度，避免递归
 */
int correct_handle_fault(struct vm_area_struct *vma, unsigned long addr)
{
    pgd_t *pgd = pgd_offset(vma->vm_mm, addr);
    p4d_t *p4d;
    pud_t *pud;

    /* 使用专门的分配函数，它们内部处理了锁 */
    p4d = p4d_alloc(vma->vm_mm, pgd, addr);
    if (!p4d)
        return VM_FAULT_OOM;

    pud = pud_alloc(vma->vm_mm, p4d, addr);
    if (!pud)
        return VM_FAULT_OOM;

    /* 后续处理... */
    return 0;
}
