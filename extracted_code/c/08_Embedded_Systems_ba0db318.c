/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 959
 * Language: c
 * Block ID: ba0db318
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void TIM2_IRQHandler(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;

        // 通知任务 (FromISR版本)
        xTaskNotifyGiveFromISR(task_handle, &xHigherPriorityTaskWoken);

        // 或者使用队列
        int data = read_sensor();
        xQueueSendFromISR(queue, &data, &xHigherPriorityTaskWoken);

        // 或者使用定时器
        xTimerPendFunctionCallFromISR(function, param,
                                       priority, &xHigherPriorityTaskWoken);
    }

    // 可能的上下文切换
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
