/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\10_Lab10_mmap.md
 * Line: 152
 * Language: c
 * Block ID: c541df8e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/sysfile.c

uint64
sys_mmap(void)
{
    uint64 addr;
    int length, prot, flags, fd, offset;
    struct file *file = 0;
    struct vma *vma;
    struct proc *p = myproc();

    // 获取参数
    if (argaddr(0, &addr) < 0 ||
        argint(1, &length) < 0 ||
        argint(2, &prot) < 0 ||
        argint(3, &flags) < 0 ||
        argint(4, &fd) < 0 ||
        argint(5, &offset) < 0)
        return -1;

    // 参数检查
    if (length <= 0 || length % PGSIZE != 0)
        return -1;
    if (offset < 0 || offset % PGSIZE != 0)
        return -1;

    // 如果不是匿名映射，需要文件
    if (!(flags & MAP_ANONYMOUS)) {
        if (fd < 0 || fd >= NOFILE || (file = p->ofile[fd]) == 0)
            return -1;

        // 检查文件权限与映射权限是否匹配
        if ((prot & PROT_READ) && !file->readable)
            return -1;
        if ((prot & PROT_WRITE) && !file->writable && (flags & MAP_SHARED))
            return -1;

        // 增加文件引用计数
        filedup(file);
    }

    // 查找空闲VMA
    vma = find_free_vma(p);
    if (!vma) {
        if (file)
            fileclose(file);
        return -1;
    }

    // 分配地址空间
    // 从进程堆顶开始查找
    addr = PGROUNDUP(p->sz);

    // 检查地址空间是否足够
    if (addr + length > MAXVA) {
        if (file)
            fileclose(file);
        return -1;
    }

    // 填充VMA
    vma->valid = 1;
    vma->addr = addr;
    vma->length = length;
    vma->prot = prot;
    vma->flags = flags;
    vma->file = file;
    vma->offset = offset;

    // 增加进程大小
    p->sz = addr + length;

    return addr;
}
