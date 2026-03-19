/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 678
 * Language: c
 * Block ID: ac26db73
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 中断: 硬件事件的软件响应
// 关键语义: 异步、快速、最小化
// ============================================================

// 中断向量表定义 (Cortex-M)
typedef void (*ISR_Handler)(void);

__attribute__((section(".isr_vector")))
const ISR_Handler vector_table[] = {
    (ISR_Handler)&_estack,      // 栈顶指针
    Reset_Handler,              // 复位
    NMI_Handler,                // 非屏蔽中断
    HardFault_Handler,          // 硬错误
    // ...
    TIM2_IRQHandler,            // 定时器2
    TIM3_IRQHandler,            // 定时器3
    // ...
};

// 中断处理最佳实践
// 原则1: ISR必须短小精悍
// 原则2: 不能在ISR中阻塞
// 原则3: 与主程序共享数据必须volatile + 同步

// 典型ISR结构: "标记-通知"模式
static volatile uint32_t tim2_tick = 0;
static volatile uint8_t tim2_flag = 0;

void TIM2_IRQHandler(void) {
    // 1. 清除中断标志 (必须先做，防止重入)
    TIM2->SR &= ~TIM_SR_UIF;

    // 2. 最小工作: 更新时间戳，设置标志
    tim2_tick++;
    tim2_flag = 1;

    // 3. 唤醒任务 (如果使用RTOS)
    // osSignalSet(task_id, SIG_TIM2);
}

// 主循环处理
void main_loop(void) {
    if (tim2_flag) {
        tim2_flag = 0;  // 清除标志

        // 执行耗时操作
        process_periodic_task();
    }
}
