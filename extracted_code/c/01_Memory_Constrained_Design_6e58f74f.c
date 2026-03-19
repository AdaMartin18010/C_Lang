/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 2898
 * Language: c
 * Block ID: 6e58f74f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 传感器节点固件 - 内存受限系统完整示例
 *
 * 目标平台: STM32F103C8 (20KB RAM, 64KB Flash)
 * 功能: 多传感器采集、LoRa传输、低功耗管理
 * ============================================================================
 */

/* 主要源文件:
 * - main.c        : 主程序入口和主循环
 * - config.h      : 系统配置
 * - sensors.c     : 传感器驱动
 * - lora.c        : LoRa通信
 * - power.c       : 电源管理
 * - memory.c      : 内存管理
 * - utils.c       : 工具函数
 */

/* main.c */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "config.h"
#include "types.h"
#include "hal.h"

/* 全局状态 */
static struct {
    system_state_t state;
    uint32_t last_tx_time;
    uint32_t last_sample_time;
    uint16_t tx_seq_num;
    uint8_t device_id[4];
} system_state;

/* 静态缓冲区 */
static uint8_t tx_buffer[TX_BUFFER_SIZE];
static sensor_sample_t *sample_buffer[SENSOR_COUNT * 2];
static uint8_t sample_count = 0;

/* 初始化 */
static void system_init(void)
{
    /* 硬件初始化 */
    system_clock_init();
    uart_init(115200);

    #if ENABLE_DEBUG
    uart_send_string("Sensor Node v");
    uart_send('0' + FIRMWARE_VERSION_MAJOR);
    uart_send('.');
    uart_send('0' + FIRMWARE_VERSION_MINOR);
    uart_send_string("\r\n");
    #endif

    /* 读取设备ID (从UID) */
    extern uint32_t UID[3];
    system_state.device_id[0] = UID[0] & 0xFF;
    system_state.device_id[1] = (UID[0] >> 8) & 0xFF;
    system_state.device_id[2] = UID[1] & 0xFF;
    system_state.device_id[3] = (UID[1] >> 8) & 0xFF;

    /* 初始化模块 */
    sensors_init();

    #if ENABLE_LORA
    lora_init();
    #endif

    stack_monitor_init();

    system_state.state = STATE_IDLE;
    system_state.tx_seq_num = 0;
}

/* 采样任务 */
static void sample_task(uint32_t current_time)
{
    if (current_time - system_state.last_sample_time < SAMPLE_INTERVAL_MS) {
        return;
    }
    system_state.last_sample_time = current_time;

    /* 采集所有传感器 */
    for (int i = 0; i < SENSOR_COUNT; i++) {
        if (sensor_should_sample(i, current_time)) {
            sensor_sample_t *sample = sensor_sample(i);
            if (sample) {
                sample->timestamp_delta = (uint16_t)(current_time / 1000);

                if (sample_count < sizeof(sample_buffer) / sizeof(sample_buffer[0])) {
                    sample_buffer[sample_count++] = sample;
                }
            }
        }
    }
}

/* 传输任务 */
static void transmit_task(uint32_t current_time)
{
    if (current_time - system_state.last_tx_time < TX_INTERVAL_MS) {
        return;
    }
    if (sample_count == 0) {
        return;
    }

    system_state.last_tx_time = current_time;
    system_state.state = STATE_TRANSMITTING;

    /* 构建数据帧 */
    data_frame_header_t *frame = (data_frame_header_t *)tx_buffer;
    frame->sync = FRAME_SYNC_BYTE;
    frame->version = FRAME_VERSION;
    frame->seq_num = system_state.tx_seq_num++;
    memcpy(frame->device_id, system_state.device_id, 4);
    frame->battery_level = get_battery_level();
    frame->rssi = get_last_rssi();
    frame->sample_count = sample_count;

    /* 复制样本数据 */
    uint8_t *payload = tx_buffer + sizeof(data_frame_header_t);
    uint8_t payload_size = sample_count * sizeof(sensor_sample_t);

    for (int i = 0; i < sample_count; i++) {
        memcpy(payload + i * sizeof(sensor_sample_t),
               sample_buffer[i], sizeof(sensor_sample_t));
        sample_free(sample_buffer[i]);
        sample_buffer[i] = NULL;
    }

    /* 计算CRC */
    uint8_t total_size = sizeof(data_frame_header_t) + payload_size;
    tx_buffer[total_size] = crc8(tx_buffer, total_size);

    /* 发送 */
    #if ENABLE_LORA
    lora_send(tx_buffer, total_size + 1);
    #endif

    sample_count = 0;
    system_state.state = STATE_IDLE;
}

/* 电源管理任务 */
static void power_task(uint32_t current_time)
{
    #if ENABLE_SLEEP
    if (system_state.state == STATE_IDLE &&
        can_enter_sleep(current_time)) {

        system_state.state = STATE_SLEEPING;

        /* 进入低功耗模式 */
        enter_sleep_mode();

        system_state.state = STATE_IDLE;
    }
    #endif
}

/* 看门狗喂狗 */
static void watchdog_task(void)
{
    #if ENABLE_WATCHDOG
    watchdog_feed();
    #endif
}

/* 主循环 */
static void main_loop(void)
{
    while (1) {
        uint32_t current_time = get_tick_ms();

        /* 执行任务 */
        sample_task(current_time);
        transmit_task(current_time);
        power_task(current_time);
        watchdog_task();

        /* 短暂延时让出CPU */
        delay_ms(1);
    }
}

/* 主函数 */
int main(void)
{
    system_init();
    main_loop();

    /* 不应到达这里 */
    return 0;
}
