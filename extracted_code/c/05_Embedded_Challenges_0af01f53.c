/*
 * Auto-generated from: 12_Practice_Exercises\05_Embedded_Challenges.md
 * Line: 66
 * Language: c
 * Block ID: 0af01f53
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 配置PA0为输入 (按键)
void button_init(void) {
    gpioa_clock_enable();
    GPIOA_MODER &= ~(3 << 0);  // 输入模式
}

// 带消抖的按键检测
bool button_pressed(void) {
    static uint32_t last_time = 0;
    static bool last_state = false;

    bool current_state = (GPIOA_IDR & (1 << 0)) != 0;
    uint32_t current_time = get_tick();  // 需要实现systick

    if (current_state != last_state &&
        current_time - last_time > 20) {  // 20ms消抖
        last_time = current_time;
        last_state = current_state;
        return current_state;
    }
    return false;
}
