/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 874
 * Language: c
 * Block ID: 27d8c3d5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// GPIO中断的高级用法
// ============================================================

// ========== STM32外部中断线(EXTI) ==========
// EXTI线0-15对应GPIO pin 0-15，同一时刻每个线只能映射一个GPIO

void stm32_exti_config(void) {
    // 使能SYSCFG时钟
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    // 配置PA0为外部中断
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;  // 上升沿触发
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置中断优先级
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

// EXTI0中断处理
void EXTI0_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_0) {
        // 处理中断
    }
}

// ========== ESP32 GPIO中断 ==========
// ESP32每个GPIO都有独立的中断能力

void esp32_gpio_interrupt(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_0),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE,  // 下降沿触发
    };
    gpio_config(&io_conf);

    // 安装ISR服务
    gpio_install_isr_service(ESP_INTR_FLAG_EDGE);

    // 添加处理函数
    gpio_isr_handler_add(GPIO_NUM_0, gpio_isr_handler, (void*)0);
}

// ========== RP2040 GPIO中断 ==========
// RP2040每个核心有独立的NVIC

void rp2040_gpio_irq(void) {
    // 配置GPIO中断
    gpio_set_irq_enabled_with_callback(0, GPIO_IRQ_EDGE_FALL, true, gpio_callback);
}

void gpio_callback(uint gpio, uint32_t events) {
    // 处理中断
    // RP2040的GPIO中断处理非常简单
}

// ========== 按键消抖的三种方法 ==========

// 方法1: 软件延时消抖 (最简单但阻塞)
bool button_read_debounced_simple(uint32_t pin) {
    if (gpio_get(pin) == 0) {  // 按键按下
        sleep_ms(20);  // 延时20ms
        return gpio_get(pin) == 0;  // 再次检测
    }
    return false;
}

// 方法2: 定时器消抖 (推荐，非阻塞)
typedef struct {
    uint32_t pin;
    uint32_t last_time;
    bool last_state;
    bool stable_state;
} DebounceButton;

void button_update(DebounceButton *btn) {
    bool current = gpio_get(btn->pin);
    uint32_t now = time_us_32();

    if (current != btn->last_state) {
        btn->last_time = now;
        btn->last_state = current;
    }

    // 状态稳定超过20ms
    if ((now - btn->last_time) > 20000) {
        btn->stable_state = current;
    }
}

// 方法3: 状态机消抖 (最完善，支持长按、双击)
typedef enum {
    BTN_IDLE,
    BTN_PRESSING,
    BTN_PRESSED,
    BTN_RELEASING
} ButtonState;

typedef struct {
    ButtonState state;
    uint32_t press_time;
    uint32_t release_time;
    bool pressed_event;
    bool released_event;
    bool long_press_event;
} ButtonFSM;

void button_fsm_update(ButtonFSM *fsm, bool raw_state) {
    uint32_t now = time_us_32();

    switch (fsm->state) {
        case BTN_IDLE:
            if (!raw_state) {  // 按下
                fsm->state = BTN_PRESSING;
                fsm->press_time = now;
            }
            break;

        case BTN_PRESSING:
            if (raw_state) {  // 抖动释放
                fsm->state = BTN_IDLE;
            } else if ((now - fsm->press_time) > 20000) {  // 确认按下
                fsm->state = BTN_PRESSED;
                fsm->pressed_event = true;
            }
            break;

        case BTN_PRESSED:
            if (!raw_state) {  // 长按检测
                if ((now - fsm->press_time) > 1000000) {  // 1秒长按
                    fsm->long_press_event = true;
                }
            } else {  // 释放
                fsm->state = BTN_RELEASING;
                fsm->release_time = now;
            }
            break;

        case BTN_RELEASING:
            if (!raw_state) {  // 抖动按下
                fsm->state = BTN_PRESSED;
            } else if ((now - fsm->release_time) > 20000) {  // 确认释放
                fsm->state = BTN_IDLE;
                fsm->released_event = true;
            }
            break;
    }
}
