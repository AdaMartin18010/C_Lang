/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\07_Lab7_Network_Driver.md
 * Line: 378
 * Language: c
 * Block ID: 4be743a6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/e1000.c

void
e1000_intr(void)
{
    // 读取中断原因寄存器 (自动清除中断)
    uint32 icr = e1000_regs[E1000_ICR / 4];

    if (icr & 0x80) {
        // 接收中断
        e1000_recv();
    }

    if (icr & 0x01) {
        // 发送中断
        // 可以在这里处理发送完成通知
    }
}
