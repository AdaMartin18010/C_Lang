/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 2130
 * Language: c
 * Block ID: cc11b89e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ❌ 错误：在缺页处理中访问用户态内存
 * 可能导致递归缺页和栈溢出
 */
int buggy_copy_from_user_in_fault(struct vm_area_struct *vma,
                                   unsigned long addr)
{
    char user_data[256];

    /* 危险：在缺页处理中访问用户态内存 */
    if (copy_from_user(user_data, (void __user *)addr, sizeof(user_data))) {
        /* 如果addr尚未映射，触发递归缺页！ */
        return -EFAULT;
    }

    /* 处理数据... */
    return 0;
}

/*
 * ✅ 正确：避免在缺页处理中触发新的缺页
 */
int correct_handle_user_page(struct vm_area_struct *vma,
                              unsigned long addr)
{
    struct page *page;
    void *kaddr;
    char buffer[256];

    /* 1. 确保页已分配和映射 */
    fault = handle_mm_fault(vma, addr, FAULT_FLAG_WRITE);
    if (fault & VM_FAULT_ERROR)
        return -EFAULT;

    /* 2. 获取页 */
    page = follow_page(vma, addr, FOLL_GET);
    if (!page)
        return -EFAULT;

    /* 3. 内核映射访问 */
    kaddr = kmap_local_page(page);

    /* 4. 安全地复制数据 */
    memcpy(buffer, kaddr + (addr & ~PAGE_MASK), sizeof(buffer));

    /* 5. 清理 */
    kunmap_local(kaddr);
    put_page(page);

    return 0;
}
