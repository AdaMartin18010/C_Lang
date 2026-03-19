/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 1221
 * Language: c
 * Block ID: bae79f1a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 栈使用监控
void check_stack_usage(void) {
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    uint32_t ulTotalRunTime;

    uxArraySize = uxTaskGetNumberOfTasks();
    pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

    if (pxTaskStatusArray != NULL) {
        uxArraySize = uxTaskGetSystemState(pxTaskStatusArray,
                                            uxArraySize, &ulTotalRunTime);

        printf("Task Stack High Water Mark:\n");
        for (x = 0; x < uxArraySize; x++) {
            printf("  %s: %u words free\n",
                pxTaskStatusArray[x].pcTaskName,
                pxTaskStatusArray[x].usStackHighWaterMark);
        }

        vPortFree(pxTaskStatusArray);
    }
}

// 堆使用监控
void check_heap_usage(void) {
    size_t free = xPortGetFreeHeapSize();
    size_t min_free = xPortGetMinimumEverFreeHeapSize();

    printf("Heap: free=%u, min_ever=%u\n", free, min_free);
}

// 栈溢出钩子
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    printf("STACK OVERFLOW in task: %s\n", pcTaskName);
    while (1);  // 停止
}
