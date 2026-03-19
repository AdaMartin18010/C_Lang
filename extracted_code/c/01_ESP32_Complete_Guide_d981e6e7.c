/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 3868
 * Language: c
 * Block ID: d981e6e7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 常见原因和解决 */

/* 1. 堆栈溢出 */
/* 解决: 增加任务堆栈大小 */
xTaskCreate(task_func, "task", 4096, NULL, 5, NULL);  /* 增加堆栈 */

/* 2. 看门狗超时 */
/* 解决: 在长时间操作中加入喂狗或让出 CPU */
while (processing) {
    /* 长时间处理 */
    vTaskDelay(pdMS_TO_TICKS(1));  /* 让出 CPU */
    esp_task_wdt_reset();           /* 喂狗 */
}

/* 3. 内存访问越界 */
/* 解决: 启用堆栈保护检查 */
// sdkconfig: CONFIG_COMPILER_STACK_CHECK_MODE_STRONG=y
