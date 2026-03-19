/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 383
 * Language: c
 * Block ID: f7a23bf4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Cortex-M 优先级配置
// 优先级数值越小，优先级越高

void NVIC_Init(void) {
    // 配置SysTick为最低优先级
    NVIC_SetPriority(SysTick_IRQn, 15);

    // 配置定时器2为中等优先级
    NVIC_SetPriority(TIM2_IRQn, 5);

    // 配置按键中断为高优先级
    NVIC_SetPriority(EXTI0_IRQn, 2);

    // 使能中断
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_EnableIRQ(EXTI0_IRQn);
}

// 关键区保护
uint32_t primask;

void enter_critical(void) {
    primask = __get_PRIMASK();
    __disable_irq();
}

void exit_critical(void) {
    __set_PRIMASK(primask);
}

// 使用
enter_critical();
// 访问共享资源
exit_critical();
