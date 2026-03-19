/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 300
 * Language: c
 * Block ID: 711ebec8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 影子寄存器: 硬件自动同步，避免中间状态
// ============================================================

// PWM周期配置示例 (STM32)
// TIM_ARR: 自动重装载寄存器 (有影子寄存器)
// 物理意义: 改变周期值时，当前周期不会被打断，新值在下周期生效

typedef struct {
    volatile uint32_t CR1;   // 控制寄存器1
    volatile uint32_t CR2;   // 控制寄存器2
    volatile uint32_t SMCR;  // 从模式控制
    volatile uint32_t DIER;  // DMA/中断使能
    volatile uint32_t SR;    // 状态寄存器
    volatile uint32_t EGR;   // 事件生成
    volatile uint32_t CCMR1; // 捕获/比较模式1
    volatile uint32_t CCMR2; // 捕获/比较模式2
    volatile uint32_t CCER;  // 捕获/比较使能
    volatile uint32_t CNT;   // 计数器 (实时值)
    volatile uint32_t PSC;   // 预分频器 (有影子)
    volatile uint32_t ARR;   // 自动重装载 (有影子)
    // ...
} TIM_TypeDef;

// 配置PWM频率 (安全，不会导致毛刺)
void pwm_set_frequency(TIM_TypeDef *tim, uint32_t freq_hz) {
    uint32_t period = SYSTEM_CLOCK / freq_hz - 1;

    // 写入ARR，但新值不会立即生效
    // 硬件自动: 当前计数周期完成后，才加载影子寄存器
    tim->ARR = period;

    // 生成更新事件，立即加载影子寄存器 (如果需要)
    tim->EGR = (1U << 0);  // UG位
}

// 软件层面的双缓冲 (类似概念)
typedef struct {
    uint32_t active;    // 当前生效值
    uint32_t shadow;    // 待生效值
    volatile uint8_t update_pending;  // 更新标志
} DoubleBuffer;

void double_buffer_update(DoubleBuffer *db, uint32_t new_value) {
    db->shadow = new_value;           // 写入影子
    db->update_pending = 1;           // 标记待更新
}

// 在合适的时机 (如VSYNC中断) 切换
void double_buffer_commit(DoubleBuffer *db) {
    if (db->update_pending) {
        db->active = db->shadow;      // 影子->生效
        db->update_pending = 0;
    }
}
