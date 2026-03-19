/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Systems.md
 * Line: 255
 * Language: c
 * Block ID: 4f3e77e6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// NVIC（嵌套向量中断控制器）配置

typedef struct {
    volatile uint32_t ISER[8];      // 中断使能
    uint32_t RESERVED0[24];
    volatile uint32_t ICER[8];      // 中断清除使能
    uint32_t RSERVED1[24];
    volatile uint32_t ISPR[8];      // 中断挂起
    uint32_t RESERVED2[24];
    volatile uint32_t ICPR[8];      // 中断清除挂起
    uint32_t RESERVED3[24];
    volatile uint32_t IABR[8];      // 中断活动位
    uint32_t RESERVED4[56];
    volatile uint8_t  IP[240];      // 中断优先级
    uint32_t RESERVED5[644];
    volatile uint32_t STIR;         // 软件触发中断
} NVIC_Type;

#define NVIC    ((NVIC_Type *)0xE000E100)

// 中断使能
void nvic_enable_irq(uint8_t irqn) {
    NVIC->ISER[irqn >> 5] = (1U << (irqn & 0x1F));
}

// 中断禁用
void nvic_disable_irq(uint8_t irqn) {
    NVIC->ICER[irqn >> 5] = (1U << (irqn & 0x1F));
}

// 设置优先级（0最高，15最低）
void nvic_set_priority(uint8_t irqn, uint8_t priority) {
    NVIC->IP[irqn] = (priority << 4);  // 高4位有效
}

// 中断处理示例：定时器中断
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {  // 更新中断标志
        TIM2->SR &= ~TIM_SR_UIF;   // 清除标志

        // 用户代码
        g_tick_count++;
    }
}

// 中断向量表
typedef void (*irq_handler_t)(void);

__attribute__((section(".isr_vector")))
const irq_handler_t g_isr_vector[] = {
    (irq_handler_t)&_estack,    // 栈顶
    Reset_Handler,               // 复位
    NMI_Handler,
    HardFault_Handler,
    // ... 其他系统异常
    [TIM2_IRQn + 16] = TIM2_IRQHandler,  // 外设中断
};
