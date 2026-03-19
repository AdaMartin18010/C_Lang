/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\10_Lab10_mmap.md
 * Line: 440
 * Language: c
 * Block ID: b53ad13b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/proc.c

static struct proc*
allocproc(void)
{
    // ... 原有代码 ...
}

// 在fork中复制VMA
int
fork(void)
{
    // ... 原有代码 ...

    // 复制父进程的VMA到子进程
    for (int i = 0; i < NVMA; i++) {
        if (p->vmas[i].valid) {
            np->vmas[i] = p->vmas[i];
            if (np->vmas[i].file)
                filedup(np->vmas[i].file);
        }
    }

    // ... 复制页表等 ...

    return pid;
}
