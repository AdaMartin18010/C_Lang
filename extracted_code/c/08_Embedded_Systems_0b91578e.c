/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 990
 * Language: c
 * Block ID: 0b91578e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// FreeRTOSConfig.h 配置

#define configTOTAL_HEAP_SIZE           (16 * 1024)  // 16KB堆
#define configUSE_MALLOC_FAILED_HOOK    1
#define configCHECK_FOR_STACK_OVERFLOW  2

// 任务栈大小
#define configMINIMAL_STACK_SIZE        (128)

// 创建任务时指定栈大小
xTaskCreate(vTask, "Name", 512, param, priority, &handle);

// 动态内存分配
void *buffer = pvPortMalloc(256);
if (buffer != NULL) {
    // 使用
    vPortFree(buffer);
}

// 静态分配 (推荐用于安全关键系统)
static StaticTask_t task_buffer;
static StackType_t stack_buffer[256];

TaskHandle_t handle = xTaskCreateStatic(
    vTask, "Name", 256, param, priority,
    stack_buffer, &task_buffer
);
