/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\01_Interrupt_Controller.md
 * Line: 539
 * Language: c
 * Block ID: 32bf28a1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux内核中断优先级管理
 * include/linux/interrupt.h
 */

// IRQ描述符中的优先级相关字段
struct irq_desc {
    struct irq_data     irq_data;
    unsigned int        __percpu *kstat_irqs;
    irq_flow_handler_t  handle_irq;
    struct irqaction    *action;
    unsigned int        status_use_accessors;

    // 亲和性和优先级设置
    cpumask_var_t       affinity;
    cpumask_var_t       effective_affinity;
    unsigned int        affinity_notify;

    // 中断线程化相关
    struct task_struct  *threads;
    struct irqaction    *threads_oneshot;
};

// 设置中断亲和性 (绑定到特定CPU)
int irq_set_affinity(unsigned int irq, const struct cpumask *cpumask)
{
    struct irq_desc *desc = irq_to_desc(irq);

    if (!desc)
        return -EINVAL;

    // 更新亲和性掩码
    cpumask_copy(desc->irq_data.affinity, cpumask);

    // 通知中断控制器更新
    if (desc->irq_data.chip->irq_set_affinity)
        return desc->irq_data.chip->irq_set_affinity(&desc->irq_data,
                                                      cpumask, false);
    return -EINVAL;
}

// IRQF_* 标志定义中断优先级/类型
#define IRQF_TRIGGER_RISING     0x00000001
#define IRQF_TRIGGER_FALLING    0x00000002
#define IRQF_TRIGGER_HIGH       0x00000004
#define IRQF_TRIGGER_LOW        0x00000008
#define IRQF_TRIGGER_MASK       (IRQF_TRIGGER_HIGH | IRQF_TRIGGER_LOW | \
                                 IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING)
#define IRQF_SHARED             0x00000080   // 可共享中断
#define IRQF_IRQPOLL            0x00001000   // 用于IRQ轮询
#define IRQF_ONESHOT            0x00002000   // 硬中断处理完后屏蔽，直到线程处理完
#define IRQF_NO_SUSPEND         0x00004000   // 系统挂起期间不禁用
#define IRQF_FORCE_RESUME       0x00008000   // 系统恢复时强制使能
#define IRQF_NO_THREAD          0x00010000   // 禁止线程化
#define IRQF_EARLY_RESUME       0x00020000   // 系统恢复时提前使能
#define IRQF_COND_SUSPEND       0x00040000   // 条件挂起
