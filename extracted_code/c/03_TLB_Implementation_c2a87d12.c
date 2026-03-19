/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\03_TLB_Implementation.md
 * Line: 789
 * Language: c
 * Block ID: c2a87d12
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux TLB Shootdown实现
 */

#include <linux/smp.h>
#include <linux/mm.h>
#include <asm/tlbflush.h>

/*
 * TLB Shootdown核心函数
 */

/*
 * 本地TLB刷新
 */
void local_flush_tlb(void) {
    /*
     * x86: 重新加载CR3或使用INVPCID
     * CR3 reload会刷新所有非全局TLB条目
     */
    native_write_cr3(native_read_cr3());
}

/*
 * 刷新指定虚拟地址的TLB条目 (本地)
 */
void local_flush_tlb_page(unsigned long addr) {
    /*
     * x86: 使用INVLPG指令
     * 仅使指定虚拟地址的TLB条目失效
     */
    asm volatile("invlpg (%0)" :: "r" (addr) : "memory");
}

/*
 * SMP TLB Shootdown
 * 当页表修改后，需要通知其他CPU刷新TLB
 */

// 每个CPU的TLB Shootdown统计
typedef struct {
    unsigned long tlb_flush_count;
    unsigned long tlb_shootdown_count;
} tlb_stats_t;

tlb_stats_t tlb_stats[NR_CPUS];

/*
 * TLB Shootdown IPI处理函数
 * 在目标CPU上执行
 */
void smp_invalidate_interrupt(struct pt_regs *regs) {
    /*
     * 1. 从共享数据结构读取需要刷新的地址
     * 2. 执行本地TLB刷新
     * 3. 通知发起者完成
     */

    struct mm_struct *mm = current->mm;

    if (mm->tlb_flush_all) {
        // 刷新整个TLB
        local_flush_tlb();
        tlb_stats[smp_processor_id()].tlb_flush_count++;
    } else {
        // 刷新指定地址
        for (int i = 0; i < mm->tlb_flush_count; i++) {
            local_flush_tlb_page(mm->tlb_flush_addrs[i]);
        }
        tlb_stats[smp_processor_id()].tlb_shootdown_count++;
    }

    // 确认完成
    ack_APIC_irq();
}

/*
 * 发起TLB Shootdown
 */
void flush_tlb_others(const struct cpumask *cpumask,
                      struct mm_struct *mm,
                      unsigned long addr) {
    /*
     * 1. 设置共享数据结构
     * 2. 向目标CPU发送IPI (处理器间中断)
     * 3. 等待所有目标CPU确认
     */

    // 设置需要刷新的地址
    mm->tlb_flush_all = (addr == 0);  // addr=0表示刷新全部
    if (!mm->tlb_flush_all) {
        mm->tlb_flush_addrs[0] = addr;
        mm->tlb_flush_count = 1;
    }

    // 发送IPI到目标CPU (不包括自己)
    smp_call_function_many(cpumask, smp_invalidate_interrupt, mm, 1);
}

/*
 * 完整TLB刷新流程 (munmap, mprotect等使用)
 */
void flush_tlb_mm_range(struct mm_struct *mm,
                        unsigned long start,
                        unsigned long end,
                        unsigned long stride) {
    int cpu = get_cpu();
    cpumask_t cpumask;

    /*
     * 步骤1: 本地刷新
     */
    if (stride == PAGE_SIZE) {
        // 刷新单个页
        local_flush_tlb_page(start);
    } else {
        // 刷新范围
        for (unsigned long addr = start; addr < end; addr += stride) {
            local_flush_tlb_page(addr);
        }
    }

    /*
     * 步骤2: 检查是否需要向其他CPU发送shootdown
     */
    cpumask_clear(&cpumask);

    // 遍历所有使用该mm的CPU
    for_each_online_cpu(cpu) {
        if (cpu == smp_processor_id())
            continue;

        if (per_cpu(cpu_tlbstate.loaded_mm, cpu) == mm) {
            cpumask_set_cpu(cpu, &cpumask);
        }
    }

    /*
     * 步骤3: 发送IPI进行shootdown
     */
    if (!cpumask_empty(&cpumask)) {
        flush_tlb_others(&cpumask, mm, start);
    }

    put_cpu();
}

/*
 * 优化的批量TLB Shootdown
 * Linux使用延迟TLB Shootdown来减少IPI开销
 */

struct tlb_batch {
    struct mm_struct *mm;
    unsigned long vaddrs[TLB_BATCH_SIZE];
    int nr;
};

DEFINE_PER_CPU(struct tlb_batch, tlb_batch);

/*
 * 批量添加TLB刷新请求
 */
void tlb_batch_add(struct mm_struct *mm, unsigned long vaddr) {
    struct tlb_batch *batch = &get_cpu_var(tlb_batch);

    batch->mm = mm;
    batch->vaddrs[batch->nr++] = vaddr;

    if (batch->nr >= TLB_BATCH_SIZE) {
        // 批量刷新
        tlb_batch_flush(batch);
    }

    put_cpu_var(tlb_batch);
}

/*
 * 执行批量刷新
 */
void tlb_batch_flush(struct tlb_batch *batch) {
    if (batch->nr == 0)
        return;

    // 本地批量刷新
    for (int i = 0; i < batch->nr; i++) {
        local_flush_tlb_page(batch->vaddrs[i]);
    }

    // 发送单次IPI通知其他CPU刷新所有地址
    cpumask_t cpumask;
    // ... 收集需要通知的CPU

    if (!cpumask_empty(&cpumask)) {
        smp_call_function_many(&cpumask, smp_invalidate_interrupt,
                               batch->mm, 1);
    }

    batch->nr = 0;
}
