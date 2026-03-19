/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 2122
 * Language: c
 * Block ID: 1cd38afb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file dma_basic.c
 * @brief DMA 基础示例 - 内存复制
 */

#include "pico/stdlib.h"
#include "hardware/dma.h"
#include <string.h>
#include <stdio.h>

#define BUFFER_SIZE     1024

uint32_t source_buffer[BUFFER_SIZE];
uint32_t dest_buffer[BUFFER_SIZE];

void dma_memcopy_example(void) {
    // 初始化源数据
    for (int i = 0; i < BUFFER_SIZE; i++) {
        source_buffer[i] = i * 0x11111111;
    }

    // 申请 DMA 通道
    int channel = dma_claim_unused_channel(true);

    // 配置 DMA
    dma_channel_config config = dma_channel_get_default_config(channel);

    // 32 位传输
    channel_config_set_transfer_data_size(&config, DMA_SIZE_32);

    // 读取地址递增
    channel_config_set_read_increment(&config, true);

    // 写入地址递增
    channel_config_set_write_increment(&config, true);

    // 配置传输
    dma_channel_configure(
        channel,
        &config,
        dest_buffer,        // 目标地址
        source_buffer,      // 源地址
        BUFFER_SIZE,        // 传输数量
        true                // 立即启动
    );

    // 等待完成
    dma_channel_wait_for_finish_blocking(channel);

    // 验证
    bool match = true;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (source_buffer[i] != dest_buffer[i]) {
            match = false;
            printf("Mismatch at %d: src=%08X, dst=%08X\n",
                   i, source_buffer[i], dest_buffer[i]);
            break;
        }
    }

    printf("DMA 内存复制: %s\n", match ? "成功" : "失败");

    // 释放通道
    dma_channel_unclaim(channel);
}

// DMA 链式传输示例
void dma_chained_example(void) {
    uint32_t buf1[256], buf2[256], buf3[256];

    // 初始化
    for (int i = 0; i < 256; i++) {
        buf1[i] = i;
    }

    // 申请两个通道
    int chan_a = dma_claim_unused_channel(true);
    int chan_b = dma_claim_unused_channel(true);

    // 配置通道 B (buf2 -> buf3)
    dma_channel_config cfg_b = dma_channel_get_default_config(chan_b);
    channel_config_set_transfer_data_size(&cfg_b, DMA_SIZE_32);
    channel_config_set_read_increment(&cfg_b, true);
    channel_config_set_write_increment(&cfg_b, true);

    dma_channel_configure(
        chan_b,
        &cfg_b,
        buf3,
        buf2,
        256,
        false  // 不立即启动，等链式触发
    );

    // 配置通道 A (buf1 -> buf2)，链式到通道 B
    dma_channel_config cfg_a = dma_channel_get_default_config(chan_a);
    channel_config_set_transfer_data_size(&cfg_a, DMA_SIZE_32);
    channel_config_set_read_increment(&cfg_a, true);
    channel_config_set_write_increment(&cfg_a, true);
    channel_config_set_chain_to(&cfg_a, chan_b);  // 完成后触发通道 B

    dma_channel_configure(
        chan_a,
        &cfg_a,
        buf2,
        buf1,
        256,
        true  // 启动通道 A
    );

    // 等待两个通道完成
    dma_channel_wait_for_finish_blocking(chan_b);

    // 验证
    bool match = true;
    for (int i = 0; i < 256; i++) {
        if (buf1[i] != buf3[i]) {
            match = false;
            break;
        }
    }

    printf("DMA 链式传输: %s\n", match ? "成功" : "失败");

    dma_channel_unclaim(chan_a);
    dma_channel_unclaim(chan_b);
}

// DMA 中断示例
volatile bool dma_complete = false;

void dma_irq_handler(void) {
    // 清除中断标志
    if (dma_hw->ints0 & (1u << 0)) {
        dma_hw->ints0 = 1u << 0;
        dma_complete = true;
    }
}

void dma_interrupt_example(void) {
    uint32_t src[100], dst[100];

    for (int i = 0; i < 100; i++) {
        src[i] = 0xDEADBEEF + i;
    }

    int channel = dma_claim_unused_channel(true);

    // 配置中断
    dma_channel_set_irq0_enabled(channel, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    // 配置 DMA
    dma_channel_config config = dma_channel_get_default_config(channel);
    channel_config_set_transfer_data_size(&config, DMA_SIZE_32);
    channel_config_set_read_increment(&config, true);
    channel_config_set_write_increment(&config, true);

    dma_complete = false;
    dma_channel_configure(
        channel,
        &config,
        dst,
        src,
        100,
        true
    );

    // 等待中断
    while (!dma_complete) {
        tight_loop_contents();
    }

    printf("DMA 中断完成\n");

    irq_set_enabled(DMA_IRQ_0, false);
    dma_channel_unclaim(channel);
}

int main(void) {
    stdio_init_all();
    sleep_ms(2000);

    printf("\n=== DMA 示例 ===\n\n");

    dma_memcopy_example();
    dma_chained_example();
    dma_interrupt_example();

    printf("\n所有测试完成!\n");

    while (1) tight_loop_contents();
}
