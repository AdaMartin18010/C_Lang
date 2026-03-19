/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\01_Interrupt_Controller.md
 * Line: 179
 * Language: c
 * Block ID: 4912b605
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux内核 8259A 初始化代码
 * arch/x86/kernel/i8259.c
 */

static void __init init_8259A(int auto_eoi)
{
    unsigned long flags;

    raw_spin_lock_irqsave(&i8259A_lock, flags);

    // ICW1: 边沿触发，级联，需要ICW4
    outb(0x11, PIC_MASTER_CMD);
    io_delay();
    outb(0x11, PIC_SLAVE_CMD);
    io_delay();

    // ICW2: 中断向量偏移
    // 主片: 0x20 (32), 从片: 0x28 (40)
    outb(IRQ0_VECTOR, PIC_MASTER_DATA);
    io_delay();
    outb(IRQ8_VECTOR, PIC_SLAVE_DATA);
    io_delay();

    // ICW3: 级联设置
    // 主片: IR2连接从片 (0x04)
    // 从片: 连接到主片IR2 (0x02)
    outb(0x04, PIC_MASTER_DATA);  // 主片IR2有从片
    io_delay();
    outb(0x02, PIC_SLAVE_DATA);   // 从片连接到主片IR2
    io_delay();

    // ICW4: 8086模式，正常EOI，非缓冲
    outb(auto_eoi ? 0x03 : 0x01, PIC_MASTER_DATA);
    io_delay();
    outb(auto_eoi ? 0x03 : 0x01, PIC_SLAVE_DATA);
    io_delay();

    // OCW1: 屏蔽所有中断 (IMR = 0xFF)
    outb(0xff, PIC_MASTER_DATA);
    io_delay();
    outb(0xff, PIC_SLAVE_DATA);
    io_delay();

    raw_spin_unlock_irqrestore(&i8259A_lock, flags);
}

// EOI (End of Interrupt) 发送
static inline void pic_eoi(int irq)
{
    if (irq >= 8) {
        // 从片中断，先发送从片EOI
        outb(PIC_EOI, PIC_SLAVE_CMD);
    }
    // 发送主片EOI
    outb(PIC_EOI, PIC_MASTER_CMD);
}
