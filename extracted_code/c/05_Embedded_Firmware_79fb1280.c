/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\05_Embedded_Firmware.md
 * Line: 404
 * Language: c
 * Block ID: 79fb1280
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// power_manager.c

#include "power_manager.h"

static volatile power_mode_t current_mode = POWER_MODE_RUN;

void enter_sleep_mode(void) {
    // 配置WFI(等待中断)进入睡眠模式
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

    // 可选: 关闭某些外设时钟
    RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    __WFI();  // 等待中断

    // 唤醒后恢复时钟
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
}

void enter_stop_mode(void) {
    current_mode = POWER_MODE_STOP;

    // 配置进入STOP模式
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    // 配置电压调节器
    PWR->CR &= ~PWR_CR_PDDS;  // 进入STOP模式，非STANDBY
    PWR->CR |= PWR_CR_LPMS_STOP1;

    // 保存外设状态
    backup_peripheral_state();

    // 进入低功耗模式
    __WFI();

    // 唤醒后的恢复
    restore_peripheral_state();
    SystemClock_Restore();

    current_mode = POWER_MODE_RUN;
}

void enter_standby_mode(void) {
    // 清除唤醒标志
    PWR->CR |= PWR_CR_CWUF;

    // 配置进入STANDBY模式
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    PWR->CR |= PWR_CR_PDDS;

    // 使能WKUP引脚唤醒
    PWR->CSR |= PWR_CSR_EWUP1;

    __WFI();
    // 注意: 从STANDBY唤醒相当于复位
}

// 功耗优化: 动态电压频率调节(DVFS)
void set_cpu_frequency(uint32_t freq_mhz) {
    if (freq_mhz <= 16) {
        // 使用HSI，关闭PLL
        RCC->CFGR &= ~RCC_CFGR_SW;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
        RCC->CR &= ~RCC_CR_PLLON;
    } else {
        // 重新配置PLL
        // ... 根据目标频率调整PLL参数
    }
}
