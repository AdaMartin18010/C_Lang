/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Physical_Layer\01_GPIO_Driver.md
 * Line: 346
 * Language: c
 * Block ID: 62f6ceef
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 按键检测 - 软件消抖
 */

#define BUTTON_PIN      GPIO_PIN_0
#define BUTTON_GPIO     GPIOA
#define DEBOUNCE_MS     20

void Button_Init(void)
{
    GPIO_Config config = {
        .pin = BUTTON_PIN,
        .mode = GPIO_MODE_INPUT_PU,  /* 上拉输入 */
        .speed = GPIO_SPEED_LOW
    };

    GPIO_Init(BUTTON_GPIO, &config);
}

/* 读取按键状态 (带消抖) */
uint8_t Button_Read(void)
{
    static uint8_t last_state = 1;  /* 上拉默认为高 */
    static uint32_t last_time = 0;

    uint8_t current_state = GPIO_ReadPin(BUTTON_GPIO, BUTTON_PIN);
    uint32_t current_time = get_tick_ms();

    /* 状态变化检测 */
    if (current_state != last_state) {
        /* 消抖时间检查 */
        if ((current_time - last_time) > DEBOUNCE_MS) {
            last_state = current_state;
            last_time = current_time;

            /* 按下检测 (上拉输入，按下为低) */
            if (current_state == 0) {
                return 1;  /* 按下 */
            }
        }
    }

    return 0;  /* 无按键 */
}
