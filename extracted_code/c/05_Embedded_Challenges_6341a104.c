/*
 * Auto-generated from: 12_Practice_Exercises\05_Embedded_Challenges.md
 * Line: 455
 * Language: c
 * Block ID: 6341a104
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int task_create(void (*func)(void), uint8_t priority);
void task_delay(uint32_t ms);
void task_yield(void);
void task_suspend(int task_id);
void task_resume(int task_id);

// 启动调度器
void scheduler_start(void) {
    // 配置SysTick (1ms)
    SysTick_Config(SystemCoreClock / 1000);

    // 启动第一个任务
    // ...
}
