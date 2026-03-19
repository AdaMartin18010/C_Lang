/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\README.md
 * Line: 375
 * Language: c
 * Block ID: 378c96b5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单的传感器数据采集与无线传输
#include <stdint.h>
#include <stdio.h>

#define SENSOR_READ_INTERVAL_MS 1000
#define TRANSMIT_INTERVAL_MS    10000

typedef struct __attribute__((packed)) {
    uint16_t node_id;
    int16_t  temperature;    // 0.01 degree C
    uint16_t humidity;       // 0.01 %RH
    uint16_t battery_mv;
    uint32_t timestamp;
} SensorData;

void sensor_node_main(void)
{
    SensorData data = {
        .node_id = 0x0001,
    };

    while (1) {
        // 读取传感器
        data.temperature = read_temperature_sensor();
        data.humidity = read_humidity_sensor();
        data.battery_mv = read_battery_voltage();
        data.timestamp = get_timestamp();

        // 发送数据
        wireless_send(&data, sizeof(data));

        // 进入低功耗等待
        enter_low_power_mode(TRANSMIT_INTERVAL_MS);
    }
}
