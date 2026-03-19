/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\01_Page_Table_Operations.md
 * Line: 919
 * Language: c
 * Block ID: 42eda067
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 内存映射实现 - 模拟mmap核心逻辑
 */

# include <linux/mm.h>
# include <linux/mman.h>
# include <linux/fs.h>
# include <linux/file.h>

/*
 * 建立虚拟内存区域（VMA）
 * 模拟do_mmap核心逻辑
 */
unsigned long do_mmap_pgoff(struct file *file, unsigned long addr,
                            unsigned long len, unsigned long prot,
                            unsigned long flags, unsigned long pgoff)
{
    struct mm_struct *mm = current->mm;
    struct vm_area_struct *vma, *prev;
    unsigned long vm_flags;
    int error;

    /* 参数验证 */
    if (!len)
        return -EINVAL;

    /* 页对齐 */
    len = PAGE_ALIGN(len);
    if (!len)
        return -ENOMEM;

    /* 计算VM标志 */
    vm_flags = calc_vm_prot_bits(prot) | calc_vm_flag_bits(flags);

    /* 文件映射 */
    if (file) {
        /* 检查文件访问权限 */
        if ((prot & PROT_READ) && !(file->f_mode & FMODE_READ))
            return -EACCES;

        if ((flags & MAP_SHARED) && (prot & PROT_WRITE) &&
            !(file->f_mode & FMODE_WRITE))
            return -EACCES;

        /* 设置VM标志 */
        vm_flags |= VM_MAYREAD | VM_MAYWRITE | VM_MAYEXEC;
        if (flags & MAP_SHARED)
            vm_flags |= VM_SHARED | VM_MAYSHARE;
    } else {
        /* 匿名映射 */
        vm_flags |= VM_SHARED | VM_MAYSHARE;
    }

    /* 查找合适的虚拟地址空间 */
    addr = get_unmapped_area(file, addr, len, pgoff, flags);
    if (IS_ERR_VALUE(addr))
        return addr;

    /* 创建新的VMA */
    vma = kmem_cache_zalloc(vm_area_cachep, GFP_KERNEL);
    if (!vma)
        return -ENOMEM;

    vma->vm_mm = mm;
    vma->vm_start = addr;
    vma->vm_end = addr + len;
    vma->vm_flags = vm_flags;
    vma->vm_page_prot = vm_get_page_prot(vm_flags);
    vma->vm_pgoff = pgoff;

    if (file) {
        vma->vm_file = get_file(file);
        error = file->f_op->mmap(file, vma);
        if (error) {
            fput(file);
            kmem_cache_free(vm_area_cachep, vma);
            return error;
        }
    }

    /* 插入VMA到mm的VMA列表 */
    error = insert_vm_struct(mm, vma);
    if (error) {
        kmem_cache_free(vm_area_cachep, vma);
        return error;
    }

    /* 增加映射计数 */
    mm->total_vm += len >> PAGE_SHIFT;
    if (vm_flags & VM_LOCKED)
        mm->locked_vm += len >> PAGE_SHIFT;

    return addr;
}

/*
 * 处理文件映射缺页
 * 将文件内容读入页缓存并建立映射
 */
int do_file_fault(struct vm_area_struct *vma, unsigned long address,
                  pte_t *pte, pmd_t *pmd, unsigned int flags)
{
    struct file *file = vma->vm_file;
    struct address_space *mapping = file->f_mapping;
    struct page *page;
    pgoff_t pgoff;
    int ret = 0;

    /* 计算文件偏移 */
    pgoff = ((address - vma->vm_start) >> PAGE_SHIFT) + vma->vm_pgoff;

    /* 从页缓存获取或创建页 */
    page = find_get_page(mapping, pgoff);
    if (!page) {
        /* 页不在缓存，需要读取 */
        page = alloc_page_vma(GFP_HIGHUSER_MOVABLE, vma, address);
        if (!page)
            return VM_FAULT_OOM;

        /* 读取文件内容 */
        ret = mapping->a_ops->readpage(file, page);
        if (ret) {
            put_page(page);
            return VM_FAULT_SIGBUS;
        }

        /* 添加到页缓存 */
        add_to_page_cache_lru(page, mapping, pgoff, GFP_KERNEL);
    }

    /* 等待页就绪 */
    wait_on_page_locked(page);

    /* 建立PTE映射 */
    set_pte_at(vma->vm_mm, address, pte,
               mk_pte(page, vma->vm_page_prot));

    /* 刷新TLB */
    update_mmu_cache(vma, address, pte);

    put_page(page);
    return ret;
}
