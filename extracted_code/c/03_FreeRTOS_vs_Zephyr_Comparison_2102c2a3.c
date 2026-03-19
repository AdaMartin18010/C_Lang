/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 4941
 * Language: c
 * Block ID: 2102c2a3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* FreeRTOS MPU配置示例 */
/* FreeRTOSConfig.h */
#define configENABLE_MPU                    1
#define configENABLE_ACCESS_CONTROL_LIST    1

/* 创建受保护任务 */
void vProtectedTask(void);
static const TaskParameters_t xTaskParameters = {
    vProtectedTask,
    "Protected",
    256,
    NULL,
    1,
    xStack,
    { /* 区域配置 */
        { (void *)0x20000000, 4096, portMPU_REGION_READ_WRITE },
        { (void *)0x40000000, 1024, portMPU_REGION_READ_ONLY }
    }
};
