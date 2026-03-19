/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\03_Baremetal_Embedded.md
 * Line: 1361
 * Language: c
 * Block ID: 8f2ddfbb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// data_acquisition.c

#include "hal.h"
#include <string.h>

#define SAMPLE_RATE 1000000  // 1 MSPS
#define BUFFER_SIZE 1024
#define NUM_BUFFERS 4

// 双缓冲结构
typedef struct {
    uint16_t data[BUFFER_SIZE];
    volatile int ready;
    volatile int processing;
} dma_buffer_t;

static dma_buffer_t buffers[NUM_BUFFERS];
static volatile int write_idx = 0;
static volatile int read_idx = 0;

// ADC采样中断 (每1us触发一次)
void adc_irq_handler(void) {
    uint16_t sample = adc_read();

    dma_buffer_t *buf = &buffers[write_idx];
    static int sample_count = 0;

    buf->data[sample_count++] = sample;

    if (sample_count >= BUFFER_SIZE) {
        sample_count = 0;
        buf->ready = 1;
        buf->processing = 0;

        write_idx = (write_idx + 1) % NUM_BUFFERS;

        // 检查溢出
        if (write_idx == read_idx) {
            // 缓冲区溢出！
            error_handler(ERROR_OVERFLOW);
        }
    }

    // 清除中断标志
    adc_clear_irq();
}

// 数据处理任务
void process_data(void) {
    while (1) {
        dma_buffer_t *buf = &buffers[read_idx];

        if (!buf->ready) {
            // 等待数据
            wfi();
            continue;
        }

        // 标记为处理中
        buf->processing = 1;

        // 处理数据 (FIR滤波 + 特征提取)
        perf_start();

        int16_t filtered[BUFFER_SIZE];
        fir_filter(buf->data, filtered, BUFFER_SIZE);

        extract_features(filtered, BUFFER_SIZE);

        perf_end("Processing");

        // 标记为空闲
        buf->ready = 0;
        buf->processing = 0;

        read_idx = (read_idx + 1) % NUM_BUFFERS;
    }
}

// 优化的FIR滤波器 (使用定点运算)
void fir_filter(const uint16_t *input, int16_t *output, int len) {
    // 系数 (Q15格式)
    static const int16_t coeffs[] = {
        327, 654, 981, 1308, 1635, 1962, 2289, 2616,
        2943, 3270, 2943, 2616, 2289, 1962, 1635, 1308,
        981, 654, 327
    };
    #define NUM_TAPS (sizeof(coeffs) / sizeof(coeffs[0]))

    static int16_t delay_line[NUM_TAPS];

    for (int n = 0; n < len; n++) {
        // 移位延迟线
        for (int i = NUM_TAPS - 1; i > 0; i--) {
            delay_line[i] = delay_line[i - 1];
        }
        delay_line[0] = (int16_t)input[n] - 2048;  // 转换为有符号

        // 计算卷积
        int32_t acc = 0;
        for (int i = 0; i < NUM_TAPS; i++) {
            acc += (int32_t)delay_line[i] * coeffs[i];
        }

        // Q15格式转换回输出
        output[n] = (int16_t)(acc >> 15);
    }
}

int main(void) {
    // 初始化
    platform_init();
    plic_init();
    timer_init();
    adc_init();

    // 配置ADC中断
    plic_set_priority(IRQ_ADC, 7);  // 最高优先级
    plic_enable_irq(IRQ_ADC);

    // 启动ADC
    adc_start(SAMPLE_RATE);

    // 主循环
    process_data();

    return 0;
}
