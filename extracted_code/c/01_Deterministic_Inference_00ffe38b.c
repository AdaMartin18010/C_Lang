/*
 * Auto-generated from: 16_Embedded_AI_Agents\08_Real_Time_AI\01_Deterministic_Inference.md
 * Line: 382
 * Language: c
 * Block ID: 00ffe38b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 实时AI任务参数
typedef struct {
    uint32_t period_ms;      // 周期 (例如: 10ms)
    uint32_t wcet_ms;        // 最坏执行时间
    uint32_t deadline_ms;    // 截止时间 (通常等于周期)
    uint8_t priority;        // 优先级 (周期越短越高)
    void (*task_func)(void); // 任务函数
} rt_ai_task_t;

// AI推理任务 (例如: 物体检测)
void object_detection_task(void) {
    static uint32_t last_run = 0;
    uint32_t now = get_tick_ms();

    // 检查是否满足周期要求
    if (now - last_run >= 10) {  // 100Hz
        uint32_t start = dwt_get_cycles();

        // 执行确定性推理
        deterministic_inference(g_input, g_output);

        uint32_t elapsed_ms = (dwt_get_cycles() - start) / (SystemCoreClock / 1000);

        // 监控实际执行时间
        if (elapsed_ms > 5) {  // 如果超过50%的周期
            trigger_warning("Inference approaching deadline");
        }

        last_run = now;
    }
}

// 任务表
rt_ai_task_t g_ai_tasks[] = {
    {10, 5, 10, 1, object_detection_task},    // 高优先级
    {50, 10, 50, 2, lane_detection_task},     // 中优先级
    {100, 20, 100, 3, traffic_sign_task},     // 低优先级
};
