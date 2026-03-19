/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 814
 * Language: c
 * Block ID: d5f61567
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file pio_dht22.c
 * @brief 使用 PIO 读取 DHT22 温湿度传感器
 */

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

#define DHT22_PIN   15

// PIO 程序 - DHT22 读取
// 测量脉冲宽度来区分 0 和 1
static const uint16_t dht22_program[] = {
    // .wrap_target
    0x8080,  // 0: pull   block          ; 等待触发信号
    // wait_start:
    0x00c3,  // 1: jmp    pin, wait_start ; 等待引脚变低
    // wait_low:
    0x0042,  // 2: jmp    !pin, wait_low  ; 等待引脚变高 (响应信号)
    // wait_high:
    0x00c4,  // 3: jmp    pin, wait_high  ; 等待引脚变低 (数据开始)
    // bit_loop:
    0xe020,  // 4: set    x, 0             ; 清零计数器
    // count_low:
    0x4045,  // 5: jmp    x--, count_low   ; 计算低电平时间
    // wait_data_high:
    0x0047,  // 6: jmp    !pin, wait_data_high ; 等待引脚变高
    // count_high:
    0x4048,  // 7: jmp    x--, count_high   ; 计算高电平时间
    0x6008,  // 8: in     x, 8              ; 输入计数值
    // wait_data_low:
    0x00ca,  // 9: jmp    pin, wait_data_low ; 等待下一个位
    0x0004,  // a: jmp    bit_loop          ; 继续下一位
             // .wrap
};

static const struct pio_program dht22_program_struct = {
    .instructions = dht22_program,
    .length = 11,
    .origin = -1,
};

// DHT22 数据结构
typedef struct {
    uint8_t humidity_high;
    uint8_t humidity_low;
    uint8_t temp_high;
    uint8_t temp_low;
    uint8_t checksum;
} dht22_data_t;

bool dht22_read(PIO pio, uint sm, float* temperature, float* humidity) {
    uint8_t data[5] = {0};

    // 触发 DHT22
    gpio_init(DHT22_PIN);
    gpio_set_dir(DHT22_PIN, GPIO_OUT);
    gpio_put(DHT22_PIN, 0);
    sleep_ms(1);
    gpio_put(DHT22_PIN, 1);
    sleep_us(30);
    gpio_set_dir(DHT22_PIN, GPIO_IN);
    gpio_pull_up(DHT22_PIN);

    // 启动 PIO 状态机
    pio_sm_put_blocking(pio, sm, 1);

    // 读取 40 位数据 (5 字节)
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 8; j++) {
            uint32_t val = pio_sm_get_blocking(pio, sm);
            // 根据计数值判断是 0 还是 1
            data[i] <<= 1;
            if (val < 40) {  // 阈值，根据实际调整
                data[i] |= 1;
            }
        }
    }

    // 校验和检查
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if (checksum != data[4]) {
        return false;
    }

    // 解析数据
    *humidity = ((data[0] << 8) | data[1]) / 10.0f;
    int16_t temp_raw = (data[2] << 8) | data[3];
    if (temp_raw & 0x8000) {
        temp_raw = -(temp_raw & 0x7FFF);
    }
    *temperature = temp_raw / 10.0f;

    return true;
}

int main(void) {
    stdio_init_all();

    PIO pio = pio0;
    uint sm = 0;
    uint offset;

    // 加载 PIO 程序
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(
        &dht22_program_struct, &pio, &sm, &offset,
        DHT22_PIN, 1, true
    );

    if (!success) {
        printf("DHT22 PIO 程序加载失败\n");
        return -1;
    }

    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_in_pins(&c, DHT22_PIN);
    sm_config_set_jmp_pin(&c, DHT22_PIN);
    sm_config_set_in_shift(&c, true, false, 32);

    float clkdiv = (float)clock_get_hz(clk_sys) / 1000000.0f;  // 1MHz
    sm_config_set_clkdiv(&c, clkdiv);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    printf("DHT22 传感器读取启动\n");

    while (1) {
        float temp, humidity;

        if (dht22_read(pio, sm, &temp, &humidity)) {
            printf("温度: %.1f°C, 湿度: %.1f%%\n", temp, humidity);
        } else {
            printf("DHT22 读取失败\n");
        }

        sleep_ms(2000);  // DHT22 最小采样间隔 2 秒
    }

    return 0;
}
