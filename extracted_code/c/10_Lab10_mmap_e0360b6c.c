/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\10_Lab10_mmap.md
 * Line: 371
 * Language: c
 * Block ID: e0360b6c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/sysfile.c

uint64
sys_munmap(void)
{
    uint64 addr;
    int length;
    struct proc *p = myproc();
    struct vma *vma;

    if (argaddr(0, &addr) < 0 ||
        argint(1, &length) < 0)
        return -1;

    // 参数检查
    if (addr % PGSIZE != 0 || length <= 0)
        return -1;

    // 查找包含该地址的VMA
    vma = find_vma(p, addr);
    if (!vma)
        return -1;

    // 检查范围
    if (addr < vma->addr || addr + length > vma->addr + vma->length)
        return -1;

    // 如果是共享映射且文件可写，写回修改
    if ((vma->flags & MAP_SHARED) && vma->file && vma->file->writable) {
        int offset = vma->offset + (addr - vma->addr);
        filewrite(vma->file, addr, length, offset);
    }

    // 解除页表映射
    uvmunmap(p->pagetable, addr, length / PGSIZE, 1);

    // 更新或删除VMA
    if (addr == vma->addr && length == vma->length) {
        // 完全匹配，删除整个VMA
        vma->valid = 0;
        if (vma->file) {
            fileclose(vma->file);
            vma->file = 0;
        }
    } else if (addr == vma->addr) {
        // 从头部解除映射
        vma->addr += length;
        vma->offset += length;
        vma->length -= length;
    } else if (addr + length == vma->addr + vma->length) {
        // 从尾部解除映射
        vma->length -= length;
    } else {
        // 从中间解除映射 - 需要分裂VMA
        // 简化处理：不支持
        panic("munmap: splitting VMA not supported");
    }

    return 0;
}
