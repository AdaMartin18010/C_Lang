/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\03_Lab3_Page_Tables.md
 * Line: 136
 * Language: c
 * Block ID: 2644de8e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/proc.h
struct proc {
    // ... 原有字段 ...
    pagetable_t kernel_pagetable;  // 进程的内核页表副本
};

// kernel/vm.c
// 复制内核页表到进程
pagetable_t proc_kpt_init() {
    pagetable_t kpt = uvmcreate();
    if (kpt == 0) return 0;

    // 复制内核映射 (uart, virtio, PLIC等)
    // UART
    mappages(kpt, UART0, PGSIZE, UART0, PTE_R | PTE_W);

    // Virtio MMIO
    mappages(kpt, VIRTIO0, PGSIZE, VIRTIO0, PTE_R | PTE_W);

    // PLIC
    mappages(kpt, PLIC, 0x400000, PLIC, PTE_R | PTE_W);

    // 内核代码和数据
    // ... 复制其他必要映射

    return kpt;
}

// kernel/proc.c
// 在allocproc()中初始化
static struct proc* allocproc(void) {
    // ...

    // 初始化内核页表副本
    p->kernel_pagetable = proc_kpt_init();
    if (p->kernel_pagetable == 0) {
        freeproc(p);
        release(&p->lock);
        return 0;
    }

    // ...
}
