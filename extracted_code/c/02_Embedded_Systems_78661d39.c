/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Systems.md
 * Line: 317
 * Language: c
 * Block ID: 78661d39
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 低功耗模式管理

#include <stdint.h>

typedef enum {
    RUN_MODE,           // 运行模式
    SLEEP_MODE,         // 睡眠模式（WFI/WFE）
    STOP_MODE,          // 停止模式
    STANDBY_MODE,       // 待机模式
} Power_Mode_t;

// 进入睡眠模式
void enter_sleep_mode(void) {
    // 配置唤醒源（中断或事件）

    // 执行WFI（等待中断）
    __asm__ volatile ("wfi");

    // 从中断返回后继续执行
}

// 进入停止模式
void enter_stop_mode(void) {
    // 1. 保存外设状态
    save_peripheral_state();

    // 2. 关闭非必要外设
    disable_unused_peripherals();

    // 3. 配置唤醒源
    enable_wakeup_source();

    // 4. 设置停止模式
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    // 5. 执行WFI
    __asm__ volatile ("wfi");

    // 6. 唤醒后恢复
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
    restore_peripheral_state();
}

// 时钟门控优化
void optimize_clocks(void) {
    // 禁用未使用外设的时钟
    RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOBEN |  // 如果GPIOB不用
                      RCC_AHB1ENR_GPIOCEN);  // 如果GPIOC不用

    // 降低总线时钟
    // 仅在高性能操作时提升时钟
}

// 动态电压频率调节（DVFS）
void set_performance_level(uint8_t level) {
    switch (level) {
        case 0:  // 低功耗
            set_system_clock(4000000);   // 4MHz
            set_voltage_scale(3);        // 低电压
            break;
        case 1:  // 平衡
            set_system_clock(16000000);  // 16MHz
            set_voltage_scale(2);
            break;
        case 2:  // 高性能
            set_system_clock(72000000);  // 72MHz
            set_voltage_scale(1);
            break;
    }
}
