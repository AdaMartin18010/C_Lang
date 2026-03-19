/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 258
 * Language: c
 * Block ID: d12d2f71
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 读-改-写: 嵌入式寄存器操作的基本模式
// 物理原因: 寄存器是32位整体，不能只改某几位
// ============================================================

// 标准RMW模式 (非原子，可能被中断打断)
void set_bit_rmw(volatile uint32_t *reg, uint32_t bit) {
    uint32_t tmp = *reg;    // 1. 读 (Read)
    tmp |= bit;             // 2. 改 (Modify)
    *reg = tmp;             // 3. 写 (Write)
}

// 问题: 中断可能在这里发生，修改同一寄存器，导致丢失修改
// 时间线: 主程序读 -> 中断修改寄存器 -> 主程序写 (覆盖中断的修改)

// 解决方案1: 临界区保护
void set_bit_safe(volatile uint32_t *reg, uint32_t bit) {
    disable_interrupts();   // 关中断
    uint32_t tmp = *reg;
    tmp |= bit;
    *reg = tmp;
    enable_interrupts();    // 开中断
}

// 解决方案2: 硬件位带（Cortex-M特有）
// 位带: 每个bit映射到单独的32位地址，写操作天然原子
#define BITBAND_ADDR(addr, bitnum) (((addr) & 0xF0000000) + 0x2000000 + \
                                    (((addr) & 0xFFFFF) << 5) + ((bitnum) << 2))
#define BITBAND_PTR(addr, bitnum) ((volatile uint32_t *)BITBAND_ADDR(addr, bitnum))

// 原子设置PA0 (位带)
*BITBAND_PTR(GPIOA_BASE + 0x14, 0) = 1;

// 解决方案3: 硬件RMW寄存器（如GPIO的BSRR）
// BSRR: 低16位写1置位，高16位写1清零，各自独立，原子操作
#define GPIO_SET_PIN(gpio, pin)   ((gpio)->BSRR = (1U << (pin)))       // 设置
#define GPIO_RESET_PIN(gpio, pin) ((gpio)->BSRR = (1U << ((pin) + 16))) // 清零
