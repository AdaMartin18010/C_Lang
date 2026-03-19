/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 1977
 * Language: c
 * Block ID: ab689693
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// RP2040 链式DMA示例
// 自动连续传输多个缓冲区，无需CPU干预
// ============================================================

typedef struct {
    uint8_t buffer[1024];
    uint16_t length;
    bool ready;
} DMA_Buffer;

#define NUM_BUFFERS 3

static DMA_Buffer dma_buffers[NUM_BUFFERS];
static volatile uint8_t current_buffer = 0;
static volatile uint8_t filled_buffer = 0;

// DMA链式配置
void setup_chained_dma(uint32_t src_pin) {
    int ctrl_chan = dma_claim_unused_channel(true);
    int data_chan = dma_claim_unused_channel(true);

    // 控制通道: 重新配置数据通道
    dma_channel_config ctrl_cfg = dma_channel_get_default_config(ctrl_chan);
    channel_config_set_transfer_data_size(&ctrl_cfg, DMA_SIZE_32);
    channel_config_set_read_increment(&ctrl_cfg, false);
    channel_config_set_write_increment(&ctrl_cfg, false);

    // 数据通道配置
    dma_channel_config data_cfg = dma_channel_get_default_config(data_chan);
    channel_config_set_transfer_data_size(&data_cfg, DMA_SIZE_8);
    channel_config_set_read_increment(&data_cfg, false);  // 从PIO读取
    channel_config_set_write_increment(&data_cfg, true);
    channel_config_set_dreq(&data_cfg, DREQ_PIO0_RX0);   // 由PIO触发

    // 启动链式传输
    // ...
}

// 双缓冲+乒乓模式
void ping_pong_dma_example(void) {
    // 使用两个缓冲区交替填充和处理
    uint8_t buffer_a[1024];
    uint8_t buffer_b[1024];

    volatile uint8_t *filling = buffer_a;
    volatile uint8_t *processing = NULL;

    while (1) {
        // 等待当前填充完成
        dma_channel_wait_for_finish_blocking(dma_chan);

        // 交换缓冲区
        if (filling == buffer_a) {
            processing = buffer_a;
            filling = buffer_b;
        } else {
            processing = buffer_b;
            filling = buffer_a;
        }

        // 立即启动下一次DMA到另一个缓冲区
        dma_channel_transfer_to_buffer_now(dma_chan, (void*)filling, 1024);

        // 处理刚完成的缓冲区
        process_buffer((uint8_t*)processing, 1024);
    }
}
