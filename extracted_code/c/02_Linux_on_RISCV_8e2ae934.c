/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\02_Linux_on_RISCV.md
 * Line: 685
 * Language: c
 * Block ID: 8e2ae934
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 中断亲和性设置
#include <linux/interrupt.h>

void optimize_interrupt_affinity(int irq, int cpu)
{
    struct cpumask mask;

    cpumask_clear(&mask);
    cpumask_set_cpu(cpu, &mask);

    // 绑定中断到特定CPU
    irq_set_affinity_hint(irq, &mask);
    irq_set_affinity(irq, &mask);
}

// NAPI轮询优化
static int my_napi_poll(struct napi_struct *napi, int budget)
{
    int work_done = 0;

    while (work_done < budget) {
        // 处理数据包
        if (!process_packet())
            break;
        work_done++;
    }

    if (work_done < budget) {
        napi_complete_done(napi, work_done);
        // 重新使能中断
        enable_irq(my_irq);
    }

    return work_done;
}
