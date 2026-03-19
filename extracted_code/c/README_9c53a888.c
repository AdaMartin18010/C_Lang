/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\README.md
 * Line: 403
 * Language: c
 * Block ID: 9c53a888
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// FreeRTOS 空间应用配置
#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1
#define configMAX_PRIORITIES                    8
#define configTICK_RATE_HZ                      1000
#define configMINIMAL_STACK_SIZE                256
#define configTOTAL_HEAP_SIZE                   (64 * 1024)

// 任务看门狗
void watchdog_task(void *pvParameters) {
    TickType_t last_wake = xTaskGetTickCount();

    while (1) {
        // 检查所有任务状态
        for (int i = 0; i < NUM_TASKS; i++) {
            if (task_states[i].last_ping + PING_TIMEOUT <
                xTaskGetTickCount()) {
                // 任务无响应
                log_error("Task %d unresponsive", i);
                trigger_task_restart(i);
            }
        }

        // 喂硬件看门狗
        feed_hardware_watchdog();

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(100));
    }
}

// 任务健康检查
void health_check_task(void *pvParameters) {
    while (1) {
        // 内存检查
        size_t free_heap = xPortGetFreeHeapSize();
        if (free_heap < HEAP_WARNING_THRESHOLD) {
            log_warning("Low heap: %d bytes", free_heap);
        }

        // 堆栈溢出检查
        for (int i = 0; i < NUM_TASKS; i++) {
            if (uxTaskGetStackHighWaterMark(task_handles[i]) <
                STACK_WARNING_THRESHOLD) {
                log_warning("Task %d stack low", i);
            }
        }

        // 向看门狗报告
        task_states[HEALTH_TASK_ID].last_ping = xTaskGetTickCount();

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
