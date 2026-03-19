/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\10_Lab10_mmap.md
 * Line: 472
 * Language: c
 * Block ID: ca100c7a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/proc.c

void
exit(int status)
{
    struct proc *p = myproc();

    // ... 原有代码 ...

    // 清理所有VMA
    for (int i = 0; i < NVMA; i++) {
        if (p->vmas[i].valid) {
            // 共享映射写回
            if ((p->vmas[i].flags & MAP_SHARED) &&
                p->vmas[i].file && p->vmas[i].file->writable) {
                filewrite(p->vmas[i].file,
                         p->vmas[i].addr,
                         p->vmas[i].length,
                         p->vmas[i].offset);
            }

            // 解除映射
            uvmunmap(p->pagetable,
                    p->vmas[i].addr,
                    p->vmas[i].length / PGSIZE, 1);

            // 关闭文件
            if (p->vmas[i].file)
                fileclose(p->vmas[i].file);

            p->vmas[i].valid = 0;
        }
    }

    // ... 原有代码 ...
}
