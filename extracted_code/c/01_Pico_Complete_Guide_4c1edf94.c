/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 1423
 * Language: c
 * Block ID: 4c1edf94
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file adc_dma.c
 * @brief 使用 DMA 进行连续 ADC 采样
 */

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

#define ADC_CHANNEL     0       // GPIO26
#define SAMPLE_RATE     10000   // 10kHz
#define BUFFER_SIZE     1024    // 采样缓冲区大小

// 双缓冲区
uint16_t buffer_a[BUFFER_SIZE];
uint16_t buffer_b[BUFFER_SIZE];

volatile bool buffer_ready = false;
volatile uint16_t* ready_buffer = NULL;

// DMA 通道
int dma_channel;

// DMA 中断处理
void dma_handler(void) {
    dma_hw->ints0 = 1u << dma_channel;  // 清除中断标志

    // 交换缓冲区
    if (ready_buffer == buffer_a) {
        ready_buffer = buffer_b;
        dma_channel_set_read_addr(dma_channel, buffer_b, false);
    } else {
        ready_buffer = buffer_a;
        dma_channel_set_read_addr(dma_channel, buffer_a, false);
    }

    buffer_ready = true;

    // 重新触发 DMA
    dma_channel_set_trans_count(dma_channel, BUFFER_SIZE, true);
}

void adc_dma_init(void) {
    // 初始化 ADC
    adc_init();
    adc_select_input(ADC_CHANNEL);
    adc_fifo_setup(
        true,   // 启用 FIFO
        true,   // 启用 DMA 请求
        1,      // 在至少 1 个样本时断言 DREQ
        false,  // 不使用错误位
        false   // 不右移 (保持 12-bit)
    );

    // 设置采样率
    // ADC 时钟 = 48MHz
    // 采样时间 = (cycles + 1) / 48MHz
    // 目标 10kHz -> 4800 个时钟周期每采样
    adc_set_clkdiv(4800.0f);

    // 申请 DMA 通道
    dma_channel = dma_claim_unused_channel(true);

    // 配置 DMA
    dma_channel_config config = dma_channel_get_default_config(dma_channel);

    // 从 ADC FIFO 读取 (16-bit)
    channel_config_set_transfer_data_size(&config, DMA_SIZE_16);
    channel_config_set_read_increment(&config, false);  // 固定读取地址
    channel_config_set_write_increment(&config, true);  // 写入地址递增

    //  paced by ADC DREQ
    channel_config_set_dreq(&config, DREQ_ADC);

    // 配置链式传输（双缓冲）
    channel_config_set_chain_to(&config, dma_channel);

    dma_channel_configure(
        dma_channel,
        &config,
        buffer_a,           // 写入地址
        &adc_hw->fifo,      // 读取地址
        BUFFER_SIZE,        // 传输计数
        false               // 不立即启动
    );

    // 配置 DMA 中断
    dma_channel_set_irq0_enabled(dma_channel, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);
}

void process_samples(uint16_t* buffer, size_t size) {
    // 计算平均值
    uint32_t sum = 0;
    for (size_t i = 0; i < size; i++) {
        sum += buffer[i];
    }
    float average = (sum / (float)size) * 3.3f / 4096.0f;

    // 计算峰值
    uint16_t max_val = 0, min_val = 4096;
    for (size_t i = 0; i < size; i++) {
        if (buffer[i] > max_val) max_val = buffer[i];
        if (buffer[i] < min_val) min_val = buffer[i];
    }

    float max_voltage = max_val * 3.3f / 4096.0f;
    float min_voltage = min_val * 3.3f / 4096.0f;

    printf("平均: %.3fV, 峰值: %.3fV, 谷值: %.3fV\n",
           average, max_voltage, min_voltage);
}

int main(void) {
    stdio_init_all();

    printf("ADC DMA 采样示例启动\n");

    adc_dma_init();

    // 启动 ADC 自由运行模式
    adc_run(true);

    // 启动 DMA
    dma_channel_start(dma_channel);

    while (1) {
        if (buffer_ready) {
            buffer_ready = false;

            // 处理已准备好的缓冲区
            process_samples((uint16_t*)ready_buffer, BUFFER_SIZE);
        }

        // 主循环可以做其他事情
        sleep_ms(1);
    }

    return 0;
}
