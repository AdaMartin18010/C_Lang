/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 722
 * Language: c
 * Block ID: b8b50e64
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// RP2040 DMA支持链式传输和复杂的触发模式
// ============================================================

#include "hardware/dma.h"

// 简单的内存到内存DMA
void dma_memory_copy(void *dst, const void *src, size_t len) {
    int channel = dma_claim_unused_channel(true);

    dma_channel_config c = dma_channel_get_default_config(channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);  // 32位传输
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, true);

    dma_channel_configure(
        channel,
        &c,
        dst,        // 目标
        src,        // 源
        len / 4,    // 传输计数 (32位字)
        true        // 立即启动
    );

    dma_channel_wait_for_finish_blocking(channel);
    dma_channel_unclaim(channel);
}

// ADC采样+DMA (连续采样到内存)
void adc_dma_continuous(void) {
    const uint16_t num_samples = 1000;
    uint16_t capture_buf[num_samples];

    // 配置ADC
    adc_init();
    adc_gpio_init(26);  // ADC0
    adc_select_input(0);
    adc_fifo_setup(
        true,   // 使能FIFO
        true,   // 使能DMA请求
        1,      // DREQ阈值
        false,  // 错误位
        false   // 字节移位
    );

    // 配置DMA
    int dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);

    // 从ADC FIFO读取，不增加读地址
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&cfg, false);  // ADC FIFO地址固定
    channel_config_set_write_increment(&cfg, true);  // 内存地址增加

    // 设置数据传输请求 (DREQ) - 由ADC触发
    channel_config_set_dreq(&cfg, DREQ_ADC);

    dma_channel_configure(
        dma_chan,
        &cfg,
        capture_buf,        // 目标
        &adc_hw->fifo,      // 源 (ADC FIFO)
        num_samples,
        false               // 不立即启动
    );

    // 启动DMA和ADC
    dma_channel_start(dma_chan);
    adc_run(true);  // 开始自由运行模式

    // 等待完成
    dma_channel_wait_for_finish_blocking(dma_chan);

    // 停止ADC
    adc_run(false);
    adc_fifo_drain();
}
