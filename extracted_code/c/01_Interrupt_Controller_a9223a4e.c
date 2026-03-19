/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\01_Interrupt_Controller.md
 * Line: 390
 * Language: c
 * Block ID: a9223a4e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux内核 Local APIC 初始化
 * arch/x86/kernel/apic/apic.c
 */

static void __init setup_local_APIC(void)
{
    u32 value;

    // 1. 设置Task Priority Register (TPR)
    // 设置当前任务的优先级，屏蔽优先级低于此值的中断
    apic_write(APIC_TPR, 0);

    // 2. 设置Spurious Interrupt Vector Register (SVR)
    // 使能APIC，设置伪中断向量
    value = apic_read(APIC_SVR);
    value &= ~APIC_VECTOR_MASK;
    value |= APIC_SPIV_APIC_ENABLED;  // 使能APIC
    value |= 0xFF & SPURIOUS_APIC_VECTOR;  // 设置伪中断向量
    apic_write(APIC_SVR, value);

    // 3. 设置Logical Destination Register (LDR)
    // 用于逻辑中断投递
    value = apic_read(APIC_LDR) & ~APIC_LDR_MASK;
    value |= (1 << (smp_processor_id() + 24));
    apic_write(APIC_LDR, value);

    // 4. 设置Destination Format Register (DFR)
    // 平坦模式: 所有位为1
    apic_write(APIC_DFR, APIC_DFR_FLAT);

    // 5. 配置LVT条目
    // LVT Timer: 一次性模式，向量0xEF
    value = APIC_LVT_TIMER_ONESHOT | LOCAL_TIMER_VECTOR;
    apic_write(APIC_LVTT, value);

    // LVT Error: 使能错误中断
    value = ERROR_APIC_VECTOR;
    apic_write(APIC_LVTERR, value);

    // LVT LINT0/LINT1: 配置为 ExtINT 和 NMI (如果使用)
    value = apic_read(APIC_LVT0);
    value &= ~(APIC_MODE_MASK | APIC_SEND_PENDING | APIC_INPUT_POLARITY);
    value |= APIC_LVT_MASKED;  // 默认屏蔽
    apic_write(APIC_LVT0, value);

    value = apic_read(APIC_LVT1);
    value &= ~(APIC_MODE_MASK | APIC_SEND_PENDING | APIC_INPUT_POLARITY);
    value |= APIC_LVT_MASKED;
    apic_write(APIC_LVT1, value);

    // 6. 清除错误状态寄存器
    apic_write(APIC_ESR, 0);
    apic_write(APIC_ESR, 0);

    // 7. 初始化Timer Divide Configuration
    // 设置分频比为1
    apic_write(APIC_TDCR, APIC_TDR_DIV_1);
}

// 发送IPI (Inter-Processor Interrupt) - 处理器间中断
void apic_send_IPI(int cpu, int vector)
{
    u32 icr_low, icr_high;
    unsigned long flags;

    local_irq_save(flags);

    // 设置目标CPU
    icr_high = SET_APIC_DEST_FIELD(cpu_physical_id(cpu));
    apic_write(APIC_ICR2, icr_high);

    // 设置中断向量
    icr_low = APIC_DM_FIXED | vector;
    apic_write(APIC_ICR, icr_low);

    local_irq_restore(flags);
}
