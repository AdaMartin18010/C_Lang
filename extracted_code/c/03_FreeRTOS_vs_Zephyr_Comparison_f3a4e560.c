/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 5053
 * Language: c
 * Block ID: f3a4e560
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 迁移前: FreeRTOS任务 */
void vOldTask(void *pvParameters) {
    for (;;) {
        xSemaphoreTake(xSem, portMAX_DELAY);
        /* 处理 */
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/* 迁移后: Zephyr线程 */
void new_thread(void *p1, void *p2, void *p3) {
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (1) {
        k_sem_take(&sem, K_FOREVER);
        /* 处理 */
        k_sleep(K_MSEC(100));
    }
}

K_THREAD_STACK_DEFINE(thread_stack, 2048);
static struct k_thread thread_data;

/* 启动 */
k_thread_create(&thread_data, thread_stack,
                K_THREAD_STACK_SIZEOF(thread_stack),
                new_thread, NULL, NULL, NULL,
                K_PRIO_PREEMPT(5), 0, K_NO_WAIT);
