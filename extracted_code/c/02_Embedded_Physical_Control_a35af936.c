/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 983
 * Language: c
 * Block ID: a35af936
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// PWM: 脉冲宽度调制，电机调速的标准方法
// 占空比 = 高电平时间 / 周期
// ============================================================

// 初始化PWM (以TIM1为例，带死区插入，用于半桥驱动)
void pwm_init_motor(TIM_TypeDef *tim, uint32_t freq_hz) {
    // 使能时钟
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    // 计算预分频和周期
    // PWM频率 = SystemClock / (PSC+1) / (ARR+1)
    uint32_t clock = SystemCoreClock;  // 168MHz
    uint32_t period = clock / freq_hz;  // 如1kHz -> 168000

    if (period > 65535) {
        // 需要预分频
        tim->PSC = (period / 65536);     // 预分频值
        tim->ARR = (period / (tim->PSC + 1)) - 1;
    } else {
        tim->PSC = 0;
        tim->ARR = period - 1;
    }

    // PWM模式1: 向上计数时，CNT < CCR输出有效电平
    tim->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;  // 模式1
    tim->CCMR1 |= TIM_CCMR1_OC1PE;  // 预装载使能

    // 互补输出 + 死区
    tim->CCER = TIM_CCER_CC1E | TIM_CCER_CC1NE;  // CH1和CH1N都使能

    // 死区时间配置 (DTG[7:5]=0xx => DT=DTG[7:0] * tdtg, tdtg=125ns @ 8MHz)
    // 1us死区: 1us / 125ns = 8
    tim->BDTR = (8 & 0x7F);  // DTG = 8
    tim->BDTR |= TIM_BDTR_MOE;  // 主输出使能

    // 启动
    tim->CR1 |= TIM_CR1_CEN;
}

// 设置占空比 (-1000 到 +1000，表示-100%到+100%)
// 负值: 反转，正值: 正转
void pwm_set_duty(TIM_TypeDef *tim, int16_t duty_permille) {
    // 限幅
    if (duty_permille > 1000) duty_permille = 1000;
    if (duty_permille < -1000) duty_permille = -1000;

    uint32_t arr = tim->ARR;
    uint32_t ccr;

    if (duty_permille >= 0) {
        // 正转
        ccr = (uint32_t)(duty_permille * arr / 1000);
        tim->CCR1 = ccr;
        // 如果需要方向控制，设置GPIO
        GPIO_SET_PIN(DIR_PORT, DIR_PIN);  // 假设高电平正转
    } else {
        // 反转
        ccr = (uint32_t)(-duty_permille * arr / 1000);
        tim->CCR1 = ccr;
        GPIO_RESET_PIN(DIR_PORT, DIR_PIN);
    }
}
