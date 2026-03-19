/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 338
 * Language: c
 * Block ID: 440c2080
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 定时器中断服务程序
volatile uint32_t tick_count = 0;

void TIM2_IRQHandler(void) {
    // 检查中断标志
    if (TIM2->SR & TIM_SR_UIF) {
        // 清除标志 (必须！)
        TIM2->SR &= ~TIM_SR_UIF;

        // 快速操作
        tick_count++;

        // 设置标志让主循环处理
        system_flags |= FLAG_TICK;
    }
}

// 主循环
int main(void) {
    system_init();

    while (1) {
        if (system_flags & FLAG_TICK) {
            system_flags &= ~FLAG_TICK;
            // 处理tick，可以花费较长时间
            process_1ms_tasks();
        }

        // 其他任务
        process_background_tasks();

        // 低功耗模式 (等待中断)
        __WFI();
    }
}
