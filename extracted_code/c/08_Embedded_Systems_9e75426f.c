/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 764
 * Language: c
 * Block ID: 9e75426f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// timer_driver.c

// 延时定时器
void TIM_Delay_Init(TIM_TypeDef *TIMx, uint32_t us) {
    // 使能时钟
    if (TIMx == TIM2) RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    else if (TIMx == TIM3) RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // 配置预分频和计数值
    // 假设TIM时钟为42MHz
    TIMx->PSC = 42 - 1;      // 分频到1MHz
    TIMx->ARR = us - 1;      // 计数到us

    // 单次模式
    TIMx->CR1 = TIM_CR1_OPM;
}

void TIM_Delay_us(TIM_TypeDef *TIMx, uint32_t us) {
    TIMx->ARR = us - 1;
    TIMx->CNT = 0;
    TIMx->CR1 |= TIM_CR1_CEN;
    while (TIMx->CR1 & TIM_CR1_CEN);  // 等待完成
}

// PWM输出
void TIM_PWM_Init(TIM_TypeDef *TIMx, uint8_t channel, uint32_t freq, uint8_t duty) {
    // 使能时钟
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // 配置GPIO为复用推挽
    // ... (GPIO配置代码)

    // 配置TIM为PWM模式
    uint32_t tim_clock = 84000000;
    uint32_t period = tim_clock / freq - 1;

    TIMx->PSC = 0;           // 不分频
    TIMx->ARR = period;
    TIMx->CCR1 = (period * duty) / 100;  // 占空比

    // PWM模式1
    TIMx->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE;
    TIMx->CCER = TIM_CCER_CC1E;  // 使能输出

    TIMx->CR1 = TIM_CR1_CEN;  // 启动
}
