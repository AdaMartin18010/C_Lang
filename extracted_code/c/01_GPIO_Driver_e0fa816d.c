/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Physical_Layer\01_GPIO_Driver.md
 * Line: 223
 * Language: c
 * Block ID: e0fa816d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * GPIO外部中断配置
 */

/* 中断触发方式 */
typedef enum {
    GPIO_IT_RISING = 0,            /* 上升沿 */
    GPIO_IT_FALLING,               /* 下降沿 */
    GPIO_IT_RISING_FALLING         /* 双边沿 */
} GPIO_IT_Trigger;

/* 中断配置 */
typedef struct {
    uint32_t pin;
    GPIO_IT_Trigger trigger;
    void (*callback)(void);        /* 中断回调 */
} GPIO_IT_Config;

/* 外部中断配置 (简化示例) */
void GPIO_IT_Init(const GPIO_IT_Config *config)
{
    uint32_t pin = config->pin;
    uint32_t exti_line = 0;

    /* 计算EXTI线 */
    while ((pin >> exti_line) != 1) {
        exti_line++;
    }

    /* 配置EXTI */
    EXTI->IMR |= (1U << exti_line);   /* 使能中断 */

    /* 配置触发方式 */
    switch (config->trigger) {
        case GPIO_IT_RISING:
            EXTI->RTSR |= (1U << exti_line);
            EXTI->FTSR &= ~(1U << exti_line);
            break;
        case GPIO_IT_FALLING:
            EXTI->RTSR &= ~(1U << exti_line);
            EXTI->FTSR |= (1U << exti_line);
            break;
        case GPIO_IT_RISING_FALLING:
            EXTI->RTSR |= (1U << exti_line);
            EXTI->FTSR |= (1U << exti_line);
            break;
    }

    /* NVIC配置 */
    IRQn_Type irqn = (exti_line < 5) ? (EXTI0_IRQn + exti_line) :
                     (exti_line < 10) ? EXTI9_5_IRQn : EXTI15_10_IRQn;

    NVIC_EnableIRQ(irqn);
    NVIC_SetPriority(irqn, 5);
}

/* 中断处理示例 */
void EXTI0_IRQHandler(void)
{
    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR = EXTI_PR_PR0;  /* 清除中断标志 */

        /* 用户处理 */
        button_callback();
    }
}
