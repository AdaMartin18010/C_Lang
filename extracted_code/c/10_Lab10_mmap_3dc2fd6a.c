/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\10_Lab10_mmap.md
 * Line: 117
 * Language: c
 * Block ID: 3dc2fd6a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/proc.c

// 查找可用的VMA槽
struct vma*
find_free_vma(struct proc *p)
{
    for (int i = 0; i < NVMA; i++) {
        if (!p->vmas[i].valid)
            return &p->vmas[i];
    }
    return 0;
}

// 根据地址查找VMA
struct vma*
find_vma(struct proc *p, uint64 addr)
{
    for (int i = 0; i < NVMA; i++) {
        if (p->vmas[i].valid &&
            addr >= p->vmas[i].addr &&
            addr < p->vmas[i].addr + p->vmas[i].length) {
            return &p->vmas[i];
        }
    }
    return 0;
}
