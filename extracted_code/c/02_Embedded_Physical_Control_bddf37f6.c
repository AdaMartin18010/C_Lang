/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 1051
 * Language: c
 * Block ID: bddf37f6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 步进电机: 开环位置控制，每步固定角度
// 控制参数: 步数、方向、速度(脉冲频率)
// ============================================================

typedef struct {
    GPIO_TypeDef *step_port;    // 步进引脚
    uint16_t step_pin;
    GPIO_TypeDef *dir_port;     // 方向引脚
    uint16_t dir_pin;

    int32_t position;           // 当前位置 (步数)
    int32_t target;             // 目标位置
    uint32_t speed;             // 当前速度 (脉冲/秒)
    uint32_t max_speed;
    uint32_t accel;             // 加速度
} Stepper;

// 定时器中断生成步进脉冲
void Stepper_IRQHandler(Stepper *motor) {
    if (motor->position != motor->target) {
        // 生成步进脉冲
        GPIO_SET_PIN(motor->step_port, motor->step_pin);

        // 确定方向
        if (motor->target > motor->position) {
            GPIO_SET_PIN(motor->dir_port, motor->dir_pin);
            motor->position++;
        } else {
            GPIO_RESET_PIN(motor->dir_port, motor->dir_pin);
            motor->position--;
        }

        // 延时后拉低 (最小脉冲宽度)
        delay_us(1);
        GPIO_RESET_PIN(motor->step_port, motor->step_pin);

        // 计算下一步的定时器周期 (变速控制)
        int32_t dist = motor->target - motor->position;
        if (dist < 100) {
            // 接近目标，减速
            motor->speed = motor->speed * 9 / 10;  // 减速
        } else if (motor->speed < motor->max_speed) {
            // 加速
            motor->speed += motor->accel;
        }

        // 更新定时器周期
        uint32_t period = 1000000 / motor->speed;  // us
        set_timer_period(period);
    }
}

// 移动到绝对位置
void stepper_move_to(Stepper *motor, int32_t pos) {
    motor->target = pos;
    // 使能定时器中断
    enable_timer_irq();
}
