/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 1250
 * Language: c
 * Block ID: 440947ec
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 低功耗模式: 空闲时降低功耗
// Cortex-M: Sleep, Deep Sleep, Standby
// ============================================================

// 进入睡眠模式，等待中断唤醒
void enter_sleep_mode(void) {
    // 配置系统控制寄存器
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;  // 清除DEEP位，普通睡眠

    // 执行WFI (Wait For Interrupt)
    __WFI();

    // 被中断唤醒后继续执行
}

// 进入停止模式 (更深睡眠，更低功耗)
void enter_stop_mode(void) {
    // 保存外设状态
    save_peripheral_state();

    // 配置唤醒源 (如RTC闹钟、外部中断)
    EXTI->IMR |= EXTI_IMR_MR0;  // 使能EXTI0

    // 进入Deep Sleep
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    // 配置电压调节器
    PWR->CR &= ~PWR_CR_PDDS;  // 停止模式，非待机
    PWR->CR |= PWR_CR_LPMS_STOP1;

    __WFI();

    // 唤醒后恢复
    restore_peripheral_state();
    SystemInit();  // 重新初始化时钟
}

// 主循环中的低功耗管理
void main_loop_lowpower(void) {
    while (1) {
        // 检查是否有工作要做
        if (has_pending_tasks()) {
            process_tasks();
        } else {
            // 空闲，进入低功耗
            // 设置一个定时器在下一个任务到期前唤醒
            set_wakeup_timer(get_next_task_time());
            enter_stop_mode();
        }
    }
}
