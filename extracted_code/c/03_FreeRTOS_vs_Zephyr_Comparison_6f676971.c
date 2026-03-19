/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 5088
 * Language: c
 * Block ID: 6f676971
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 迁移前: FreeRTOS队列 */
QueueHandle_t xQueue = xQueueCreate(10, sizeof(message_t));
xQueueSend(xQueue, &msg, portMAX_DELAY);
xQueueReceive(xQueue, &msg, portMAX_DELAY);

/* 迁移后: Zephyr消息队列 */
K_MSGQ_DEFINE(my_msgq, sizeof(message_t), 10, 4);
k_msgq_put(&my_msgq, &msg, K_FOREVER);
k_msgq_get(&my_msgq, &msg, K_FOREVER);
