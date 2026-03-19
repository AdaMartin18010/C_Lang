/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 2321
 * Language: c
 * Block ID: 5ddcc959
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file dma_gpio_fast.c
 * @brief 使用 DMA 实现 GPIO 高速输出
 */

#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"

#define DATA_PIN_BASE   0   // GPIO 0-7 作为数据总线
#define NUM_DATA_PINS   8
#define BUFFER_SIZE     256

// 输出数据缓冲区
uint8_t output_buffer[BUFFER_SIZE];

// 使用 DMA 将缓冲区数据快速输出到 GPIO
void dma_gpio_burst_output(void) {
    // 配置 GPIO 0-7 为输出
    for (int i = 0; i < NUM_DATA_PINS; i++) {
        gpio_init(DATA_PIN_BASE + i);
        gpio_set_dir(DATA_PIN_BASE + i, GPIO_OUT);
    }

    // 填充测试数据
    for (int i = 0; i < BUFFER_SIZE; i++) {
        output_buffer[i] = i;
    }

    // 使用 DMA 将数据写入 GPIO 输出寄存器
    // GPIO 输出寄存器地址: SIO_BASE + 0x014 (GPIO_OUT)
    int channel = dma_claim_unused_channel(true);

    dma_channel_config config = dma_channel_get_default_config(channel);
    channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
    channel_config_set_read_increment(&config, true);
    channel_config_set_write_increment(&config, false);  // 固定写入地址

    dma_channel_configure(
        channel,
        &config,
        (volatile void*)(SIO_BASE + SIO_GPIO_OUT_OFFSET),  // GPIO 输出寄存器
        output_buffer,
        BUFFER_SIZE,
        true
    );

    // 等待完成
    dma_channel_wait_for_finish_blocking(channel);

    dma_channel_unclaim(channel);

    printf("DMA GPIO 突发输出完成: %d 字节 @ %d MHz 等效\n",
           BUFFER_SIZE, (int)(clock_get_hz(clk_sys) / 1000000));
}
