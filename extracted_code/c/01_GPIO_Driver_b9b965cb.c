/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Physical_Layer\01_GPIO_Driver.md
 * Line: 298
 * Language: c
 * Block ID: b9b965cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * LED闪烁示例
 */

#define LED_PIN     GPIO_PIN_5
#define LED_GPIO    GPIOA

void LED_Init(void)
{
    GPIO_Config config = {
        .pin = LED_PIN,
        .mode = GPIO_MODE_OUTPUT_PP,
        .speed = GPIO_SPEED_LOW
    };

    GPIO_Init(LED_GPIO, &config);
}

void LED_On(void)
{
    GPIO_WritePin(LED_GPIO, LED_PIN, 1);
}

void LED_Off(void)
{
    GPIO_WritePin(LED_GPIO, LED_PIN, 0);
}

void LED_Toggle(void)
{
    GPIO_TogglePin(LED_GPIO, LED_PIN);
}

/* 主循环 */
void main_loop(void)
{
    LED_Init();

    while (1) {
        LED_Toggle();
        delay_ms(500);  /* 500ms闪烁 */
    }
}
