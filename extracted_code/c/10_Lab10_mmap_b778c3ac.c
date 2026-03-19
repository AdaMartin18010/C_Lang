/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\10_Lab10_mmap.md
 * Line: 692
 * Language: c
 * Block ID: b778c3ac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 打印VMA信息
void debug_vma(struct proc *p) {
    printf("VMAs for pid %d:\n", p->pid);
    for (int i = 0; i < NVMA; i++) {
        if (p->vmas[i].valid) {
            printf("  [%d] addr=%p len=%d prot=%x flags=%x\n",
                   i, p->vmas[i].addr, p->vmas[i].length,
                   p->vmas[i].prot, p->vmas[i].flags);
            if (p->vmas[i].file)
                printf("       file=%s offset=%d\n",
                       p->vmas[i].file->ip-> ...);
        }
    }
}

// 在页面错误时打印信息
int mmap_handler(uint64 va) {
    printf("mmap_handler: va=%p\n", va);

    struct vma *vma = find_vma(myproc(), va);
    if (vma) {
        printf("  found vma: addr=%p len=%d\n", vma->addr, vma->length);
    } else {
        printf("  no vma found!\n");
    }

    // ... 处理逻辑 ...
}
