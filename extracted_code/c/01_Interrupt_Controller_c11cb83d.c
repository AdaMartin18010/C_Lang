/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\01_Interrupt_Controller.md
 * Line: 953
 * Language: c
 * Block ID: c11cb83d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux内核通用中断处理
 * arch/x86/kernel/irq.c
 */

// 主中断处理入口
__visible unsigned int __irq_entry do_IRQ(struct pt_regs *regs)
{
    struct pt_regs *old_regs = set_irq_regs(regs);
    struct irq_desc *desc;
    unsigned vector = ~regs->orig_ax;

    // 根据向量号获取中断描述符
    desc = __this_cpu_read(vector_irq[vector]);

    if (!desc) {
        // 未处理的中断
        ack_APIC_irq();
        goto exit;
    }

    // 通用中断处理
    generic_handle_irq_desc(desc);

exit:
    set_irq_regs(old_regs);
    return 1;
}

// 通用中断处理函数
int generic_handle_irq_desc(struct irq_desc *desc)
{
    // 调用中断控制器特定的高层级处理
    return desc->handle_irq(desc);
}

// 边缘触发中断处理 (handle_edge_irq)
void handle_edge_irq(struct irq_desc *desc)
{
    // 1. 屏蔽该中断 (直到处理完成)
    mask_ack_irq(desc);

    // 2. 检查中断是否正在处理中
    if (unlikely(irqd_irq_inprogress(&desc->irq_data))) {
        // 中断重入，延迟处理
        desc->istate |= IRQS_PENDING;
        return;
    }

    // 3. 设置处理中标志
    desc->istate |= IRQS_INPROGRESS;

    // 4. 处理中断
    do {
        // 重新使能中断 (允许嵌套)
        unmask_irq(desc);

        // 调用注册的处理函数
        handle_irq_event(desc);

        // 再次屏蔽
        mask_irq(desc);

        // 检查是否有挂起的中断
    } while ((desc->istate & IRQS_PENDING) &&
             !irqd_irq_disabled(&desc->irq_data));

    // 5. 清除处理中标志
    desc->istate &= ~IRQS_INPROGRESS;

    // 6. 最终确认和使能
    ack_irq(desc);
    unmask_irq(desc);
}

// 电平触发中断处理 (handle_level_irq)
void handle_level_irq(struct irq_desc *desc)
{
    // 1. 屏蔽并确认中断
    mask_ack_irq(desc);

    // 2. 如果中断被禁用，直接返回
    if (unlikely(irqd_irq_disabled(&desc->irq_data)))
        return;

    // 3. 处理中断事件
    handle_irq_event(desc);

    // 4. 取消屏蔽 (电平触发需要等待设备释放IRQ线)
    unmask_irq(desc);
}
