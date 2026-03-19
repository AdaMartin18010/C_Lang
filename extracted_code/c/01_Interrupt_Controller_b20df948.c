/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\01_Interrupt_Controller.md
 * Line: 1181
 * Language: c
 * Block ID: b20df948
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 中断控制原语
 * arch/x86/include/asm/irqflags.h
 */

// 本地中断使能/屏蔽 (操作EFLAGS.IF位)
static inline void native_irq_disable(void)
{
    asm volatile("cli" ::: "memory");
}

static inline void native_irq_enable(void)
{
    asm volatile("sti" ::: "memory");
}

// 保存并恢复中断状态
static inline unsigned long native_save_fl(void)
{
    unsigned long flags;
    asm volatile("pushf ; pop %0" : "=g" (flags) : : "memory");
    return flags;
}

static inline void native_restore_fl(unsigned long flags)
{
    asm volatile("push %0 ; popf" : : "g" (flags) : "memory", "cc");
}

// 同时执行的操作 (原子性)
static inline unsigned long native_irq_save(void)
{
    unsigned long flags = native_save_fl();
    native_irq_disable();
    return flags;
}

// 使用场景示例
void critical_section_example(void)
{
    unsigned long flags;

    // 进入临界区，保存并屏蔽中断
    local_irq_save(flags);

    // 临界区代码 - 不会被中断打扰
    shared_counter++;
    list_add(&entry, &head);

    // 退出临界区，恢复之前的中断状态
    local_irq_restore(flags);
}

// 嵌套中断支持
void nested_irq_handler(void)
{
    // 硬件自动清除IF
    // 但某些高优先级中断可以设置IST或使用特定门描述符

    // 如果希望允许嵌套，手动使能中断
    local_irq_enable();

    // 现在可以被更高优先级中断打断
    do_work();

    // 恢复屏蔽状态
    local_irq_disable();
}
