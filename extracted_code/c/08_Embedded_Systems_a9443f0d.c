/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 1027
 * Language: c
 * Block ID: a9443f0d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// FreeRTOSConfig.h
#define configUSE_TICKLESS_IDLE     1
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP 2

// 实现钩子函数
void vApplicationSleep(TickType_t xExpectedIdleTime) {
    // 计算睡眠时间
    uint32_t sleep_ticks = xExpectedIdleTime;
    uint32_t sleep_ms = sleep_ticks * portTICK_PERIOD_MS;

    // 配置RTC唤醒
    RTC_SetWakeup(sleep_ms);

    // 进入停止模式
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

    // 唤醒后重新配置系统时钟
    SystemClock_Config();
}

// 任务设计 - 使用事件驱动
void vTask(void *pv) {
    while (1) {
        // 等待事件，阻塞任务
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // 处理事件
        process_event();

        // 完成后进入空闲，系统可进入低功耗
    }
}
