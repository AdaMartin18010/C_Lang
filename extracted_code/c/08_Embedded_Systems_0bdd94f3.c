/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 909
 * Language: c
 * Block ID: 0bdd94f3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 互斥量保护共享资源
SemaphoreHandle_t spi_mutex;

void vSPITask(void *pvParameters) {
    while (1) {
        // 获取互斥量
        if (xSemaphoreTake(spi_mutex, portMAX_DELAY) == pdTRUE) {
            // 访问SPI总线
            SPI_Transfer(data, len);

            // 释放互斥量
            xSemaphoreGive(spi_mutex);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// 二值信号量用于中断通知
SemaphoreHandle_t data_ready_sem;

void EXTI_IRQHandler(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // 通知任务
    xSemaphoreGiveFromISR(data_ready_sem, &xHigherPriorityTaskWoken);

    // 上下文切换
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void vProcessingTask(void *pvParameters) {
    while (1) {
        // 等待数据就绪
        if (xSemaphoreTake(data_ready_sem, portMAX_DELAY)) {
            // 处理数据
            process_data();
        }
    }
}
