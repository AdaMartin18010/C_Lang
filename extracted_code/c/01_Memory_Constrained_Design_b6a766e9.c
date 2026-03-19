/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 3103
 * Language: c
 * Block ID: b6a766e9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 电源管理 - 低功耗设计
 * ============================================================================ */

#include <stdint.h>
#include <stdbool.h>

/* 睡眠模式选择 */
typedef enum {
    SLEEP_MODE_IDLE,        /* 仅CPU停止 */
    SLEEP_MODE_LIGHT,       /* CPU+部分外设停止 */
    SLEEP_MODE_DEEP,        /* CPU+大部分外设停止 */
    SLEEP_MODE_STANDBY      /* 仅保留备份域 */
} sleep_mode_t;

/* 功耗统计 */
typedef struct {
    uint32_t active_time_ms;      /* 活跃时间 */
    uint32_t sleep_time_ms;       /* 睡眠时间 */
    uint32_t wake_count;          /* 唤醒次数 */
    uint32_t avg_current_ua;      /* 平均电流(微安) */
} power_stats_t;

static power_stats_t power_stats;

/* 进入睡眠模式 */
void enter_sleep_mode(sleep_mode_t mode)
{
    switch (mode) {
        case SLEEP_MODE_IDLE:
            /* 停止CPU时钟，外设继续运行 */
            __WFI();  /* Wait For Interrupt */
            break;

        case SLEEP_MODE_LIGHT:
            /* 配置唤醒源 */
            configure_wakeup_sources();

            /* 降低系统时钟 */
            reduce_system_clock();

            /* 进入低功耗模式 */
            SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
            __WFI();

            /* 恢复时钟 */
            restore_system_clock();
            break;

        case SLEEP_MODE_DEEP:
            /* 关闭不必要的外设 */
            disable_unused_peripherals();

            /* 配置RTC唤醒 */
            set_rtc_wakeup(10000);  /* 10秒后唤醒 */

            /* 进入停止模式 */
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            PWR->CR &= ~PWR_CR_PDDS;  /* 停止模式 */
            __WFI();

            /* 恢复 */
            reinit_after_wakeup();
            break;

        case SLEEP_MODE_STANDBY:
            /* 保存关键数据到备份域 */
            save_to_backup_domain();

            /* 进入待机模式 */
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            PWR->CR |= PWR_CR_PDDS;   /* 待机模式 */
            SCB->SCR |= SCB_SCR_SEVEONPEND_Msk;
            __WFI();
            break;
    }
}

/* 动态电压频率调节 (DVFS) */
typedef enum {
    CPU_SPEED_1MHZ = 1000000,
    CPU_SPEED_8MHZ = 8000000,
    CPU_SPEED_24MHZ = 24000000,
    CPU_SPEED_72MHZ = 72000000
} cpu_speed_t;

static cpu_speed_t current_speed = CPU_SPEED_72MHZ;

void set_cpu_speed(cpu_speed_t speed)
{
    if (speed == current_speed) return;

    /* 等待所有操作完成 */
    __DSB();

    switch (speed) {
        case CPU_SPEED_1MHZ:
            /* 切换到内部低速时钟 */
            RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSI;
            while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);

            /* 分频到1MHz */
            RCC->CFGR |= RCC_CFGR_HPRE_DIV8;

            /* 降低电压 */
            PWR->CR &= ~PWR_CR_VOS;
            break;

        case CPU_SPEED_72MHZ:
            /* 提高电压 */
            PWR->CR |= PWR_CR_VOS;

            /* 等待电压稳定 */
            while (!(PWR->CSR & PWR_CSR_VOSRDY));

            /* 切换到PLL */
            RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
            while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
            break;

        default:
            break;
    }

    current_speed = speed;

    /* 更新延时函数 */
    update_delay_calibration(speed);
}

/* 外设电源管理 */
typedef struct {
    uint32_t enable_mask;
    uint32_t sleep_mask;
    bool auto_disable;
} peripheral_power_t;

static peripheral_power_t periph_power = {
    .enable_mask = 0,
    .sleep_mask = 0xFFFFFFFF,  /* 默认睡眠时关闭所有 */
    .auto_disable = true
};

void peripheral_enable(uint32_t periph_id)
{
    periph_power.enable_mask |= (1 << periph_id);
    RCC->APB1ENR |= (1 << periph_id);
}

void peripheral_disable(uint32_t periph_id)
{
    periph_power.enable_mask &= ~(1 << periph_id);
    RCC->APB1ENR &= ~(1 << periph_id);
}

void peripherals_sleep_prepare(void)
{
    uint32_t disable_mask = periph_power.sleep_mask & ~periph_power.enable_mask;

    /* 关闭睡眠时不需要的外设 */
    RCC->APB1ENR &= ~disable_mask;
    RCC->APB2ENR &= ~(disable_mask >> 16);
}

void peripherals_wakeup_restore(void)
{
    /* 恢复外设时钟 */
    RCC->APB1ENR |= periph_power.enable_mask & 0xFFFF;
    RCC->APB2ENR |= (periph_power.enable_mask >> 16) & 0xFFFF;
}
