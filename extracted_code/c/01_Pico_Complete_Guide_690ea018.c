/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 2385
 * Language: c
 * Block ID: 690ea018
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file multicore_advanced.c
 * @brief 多核高级编程示例
 */

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/irq.h"
#include <stdio.h>

// FIFO 消息类型
typedef enum {
    MSG_TEMPERATURE = 1,
    MSG_PRESSURE,
    MSG_COMMAND,
    MSG_ACK
} message_type_t;

// 共享数据（使用互斥锁保护）
typedef struct {
    mutex_t lock;
    float temperature;
    float pressure;
    uint32_t sample_count;
    bool data_ready;
} shared_data_t;

shared_data_t shared_data;

// Core 1 入口函数
void core1_entry(void) {
    printf("Core 1 启动\n");

    // 配置 Core 1 中断
    multicore_fifo_clear_irq();
    irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_fifo_handler);
    irq_set_enabled(SIO_IRQ_PROC1, true);

    while (1) {
        // 模拟传感器读取
        float temp = 25.0f + (float)(rand() % 100) / 10.0f;
        float pressure = 1013.25f + (float)(rand() % 50) / 10.0f;

        // 使用互斥锁更新共享数据
        mutex_enter_blocking(&shared_data.lock);
        shared_data.temperature = temp;
        shared_data.pressure = pressure;
        shared_data.sample_count++;
        shared_data.data_ready = true;
        mutex_exit(&shared_data.lock);

        // 发送通知到 Core 0
        multicore_fifo_push_blocking(MSG_TEMPERATURE);

        sleep_ms(1000);
    }
}

// Core 1 FIFO 中断处理
void core1_fifo_handler(void) {
    uint32_t msg = multicore_fifo_pop_blocking();

    switch (msg) {
        case MSG_COMMAND:
            printf("Core 1: 收到命令\n");
            multicore_fifo_push_blocking(MSG_ACK);
            break;
    }

    multicore_fifo_clear_irq();
}

// 安全的双核数据访问
void safe_data_access_example(void) {
    mutex_init(&shared_data.lock);
    shared_data.data_ready = false;

    multicore_launch_core1(core1_entry);

    while (1) {
        // 等待数据就绪
        bool ready = false;
        while (!ready) {
            mutex_enter_blocking(&shared_data.lock);
            ready = shared_data.data_ready;
            mutex_exit(&shared_data.lock);
            sleep_ms(10);
        }

        // 读取数据
        mutex_enter_blocking(&shared_data.lock);
        float temp = shared_data.temperature;
        float pressure = shared_data.pressure;
        uint32_t count = shared_data.sample_count;
        shared_data.data_ready = false;
        mutex_exit(&shared_data.lock);

        printf("样本 #%lu: 温度=%.1f°C, 压力=%.2f hPa\n",
               count, temp, pressure);
    }
}

// 使用事件组进行多核同步
typedef volatile uint32_t event_group_t;
event_group_t core_events = 0;

#define EVENT_SENSOR_READY  (1 << 0)
#define EVENT_WIFI_CONNECTED (1 << 1)
#define EVENT_DATA_SENT     (1 << 2)

void set_event(event_group_t* events, uint32_t event) {
    uint32_t save = save_and_disable_interrupts();
    *events |= event;
    restore_interrupts(save);
}

void clear_event(event_group_t* events, uint32_t event) {
    uint32_t save = save_and_disable_interrupts();
    *events &= ~event;
    restore_interrupts(save);
}

bool wait_for_events(event_group_t* events, uint32_t wait_mask,
                     uint32_t* received, uint32_t timeout_ms) {
    absolute_time_t timeout = make_timeout_time_ms(timeout_ms);

    while (1) {
        uint32_t current;
        uint32_t save = save_and_disable_interrupts();
        current = *events;
        restore_interrupts(save);

        if (current & wait_mask) {
            *received = current & wait_mask;
            return true;
        }

        if (absolute_time_diff_us(get_absolute_time(), timeout) <= 0) {
            return false;
        }

        sleep_ms(1);
    }
}

// Core 1 任务处理
void core1_task_processor(void) {
    printf("Core 1: 任务处理器启动\n");

    while (1) {
        uint32_t received;
        if (wait_for_events(&core_events,
                           EVENT_SENSOR_READY | EVENT_WIFI_CONNECTED,
                           &received, 1000)) {

            if (received & EVENT_SENSOR_READY) {
                printf("Core 1: 处理传感器数据\n");
                clear_event(&core_events, EVENT_SENSOR_READY);
            }

            if (received & EVENT_WIFI_CONNECTED) {
                printf("Core 1: WiFi 已连接，启动数据传输\n");
                clear_event(&core_events, EVENT_WIFI_CONNECTED);
            }
        }
    }
}

int main(void) {
    stdio_init_all();
    sleep_ms(2000);

    printf("\n=== 多核高级编程 ===\n\n");

    multicore_launch_core1(core1_task_processor);

    // 模拟传感器就绪
    sleep_ms(500);
    set_event(&core_events, EVENT_SENSOR_READY);

    sleep_ms(1000);
    set_event(&core_events, EVENT_WIFI_CONNECTED);

    while (1) {
        sleep_ms(1000);
    }
}
