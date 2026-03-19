/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\10_Lab10_mmap.md
 * Line: 286
 * Language: c
 * Block ID: 3c7eadfb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/trap.c (或 vm.c)

int
mmap_handler(uint64 va)
{
    struct proc *p = myproc();
    struct vma *vma;
    char *pa;

    // 页面对齐
    va = PGROUNDDOWN(va);

    // 查找包含该地址的VMA
    vma = find_vma(p, va);
    if (!vma)
        return -1;  // 不是mmap区域

    // 检查权限
    int cause = r_scause();
    if (cause == 15) {  // Store/AMO page fault
        if (!(vma->prot & PROT_WRITE))
            return -1;  // 写保护错误
    } else if (cause == 13) {  // Load page fault
        if (!(vma->prot & PROT_READ))
            return -1;  // 读保护错误
    }

    // 分配物理页
    pa = kalloc();
    if (!pa)
        return -1;

    memset(pa, 0, PGSIZE);

    // 如果是文件映射，读取文件内容
    if (vma->file) {
        // 计算在文件中的偏移
        int offset = vma->offset + (va - vma->addr);
        int len = PGSIZE;
        if (offset + len > vma->file->ip->size)
            len = vma->file->ip->size - offset;

        if (len > 0) {
            ilock(vma->file->ip);
            readi(vma->file->ip, 0, (uint64)pa, offset, len);
            iunlock(vma->file->ip);
        }
    }

    // 设置页表权限
    int perm = PTE_U;  // 用户可访问
    if (vma->prot & PROT_READ)
        perm |= PTE_R;
    if (vma->prot & PROT_WRITE)
        perm |= PTE_W;
    if (vma->prot & PROT_EXEC)
        perm |= PTE_X;

    // 私有映射使用COW
    if ((vma->flags & MAP_PRIVATE) && (perm & PTE_W)) {
        perm = (perm & ~PTE_W) | PTE_COW;
    }

    // 建立映射
    if (mappages(p->pagetable, va, PGSIZE, (uint64)pa, perm) < 0) {
        kfree(pa);
        return -1;
    }

    // 如果是COW页面，增加引用计数
    if (perm & PTE_COW) {
        increment_ref((uint64)pa);
    }

    return 0;
}
