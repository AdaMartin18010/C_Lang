/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\02_Hard_RealTime.md
 * Line: 467
 * Language: c
 * Block ID: c9989208
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 中断管理 - 确保确定性响应
// ============================================================================

// 中断优先级 (ARM Cortex-M 示例)
// NVIC优先级数值越小优先级越高
typedef enum {
    IRQ_PRIORITY_CRITICAL = 0,      // 故障安全
    IRQ_PRIORITY_HIGH = 2,          // 传感器捕获
    IRQ_PRIORITY_MEDIUM = 5,        // 定时器
    IRQ_PRIORITY_LOW = 10,          // 通信
    IRQ_PRIORITY_LOWEST = 15        // 后台
} IRQPriority;

// 中断处理统计
typedef struct {
    uint32_t entry_time_us;
    uint32_t exit_time_us;
    uint32_t max_duration_us;
    uint32_t total_count;
    uint32_t total_duration_us;
} IRQStatistics;

IRQStatistics irq_stats[32];

// ============================================================================
// 中断入口/出口钩子
// ============================================================================

void irq_entry_hook(uint8_t irq_num) {
    irq_stats[irq_num].entry_time_us = get_microseconds();
    irq_stats[irq_num].total_count++;

    // 记录中断嵌套深度
    increment_irq_nesting();
}

void irq_exit_hook(uint8_t irq_num) {
    uint32_t exit_time = get_microseconds();
    uint32_t duration = exit_time - irq_stats[irq_num].entry_time_us;

    irq_stats[irq_num].exit_time_us = exit_time;
    irq_stats[irq_num].total_duration_us += duration;

    if (duration > irq_stats[irq_num].max_duration_us) {
        irq_stats[irq_num].max_duration_us = duration;
    }

    decrement_irq_nesting();
}

// ============================================================================
// 中断延迟测量
// ============================================================================

// 关键参数
#define MAX_IRQ_LATENCY_US      5       // 最大中断延迟要求
#define MAX_IRQ_DURATION_US     50      // 最大ISR执行时间

void measure_irq_latency(void) {
    // 使用GPIO翻转测试
    // 1. 设置GPIO触发中断
    // 2. 在ISR中翻转另一个GPIO
    // 3. 用示波器测量延迟

    // 软件测量方法
    volatile uint32_t start = get_microseconds();
    trigger_software_irq(15);  // 触发软件中断
    // ISR中会设置标志
    while (!irq_flag_set);     // 等待ISR执行
    volatile uint32_t end = get_microseconds();

    uint32_t latency = end - start;
    if (latency > MAX_IRQ_LATENCY_US) {
        log_warning(WARN_HIGH_IRQ_LATENCY, latency);
    }
}

// ============================================================================
// 任务调度延迟分析
// ============================================================================

void analyze_scheduling_latency(void) {
    // 调度延迟 = 中断响应 + 调度器执行 + 上下文切换

    // 典型值 (Cortex-M4 @ 100MHz)
    // 中断进入: 12 cycles = 0.12us
    // 中断退出: 12 cycles = 0.12us
    // 调度决策: ~50 cycles = 0.5us
    // 上下文保存: ~20 cycles = 0.2us
    // 上下文恢复: ~20 cycles = 0.2us
    // 总计: ~1us (理论最小值)

    // 实际考虑:
    // - 缓存未命中
    // - 总线竞争
    // - 内存访问等待
    // 实际: 5-20us
}
