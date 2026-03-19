/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 2577
 * Language: c
 * Block ID: 7e375ab6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file multicore_load_balance.c
 * @brief 多核负载均衡示例 - FFT 并行计算
 */

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include <math.h>
#include <complex.h>

#define FFT_SIZE        1024
#define NUM_CORES       2

// FFT 缓冲区
double complex fft_input[FFT_SIZE];
double complex fft_output[FFT_SIZE];

// 核心完成标志
volatile bool core_complete[NUM_CORES] = {false};

// 复数 FFT 旋转因子
double complex twiddle_factors[FFT_SIZE / 2];

void init_twiddle_factors(void) {
    for (int k = 0; k < FFT_SIZE / 2; k++) {
        double angle = -2.0 * M_PI * k / FFT_SIZE;
        twiddle_factors[k] = cos(angle) + I * sin(angle);
    }
}

// 位反转置换
unsigned int bit_reverse(unsigned int x, int bits) {
    unsigned int y = 0;
    for (int i = 0; i < bits; i++) {
        y = (y << 1) | (x & 1);
        x >>= 1;
    }
    return y;
}

// Core 1 FFT 计算任务
void core1_fft_task(void) {
    int start = FFT_SIZE / NUM_CORES;
    int end = FFT_SIZE;

    // 蝴蝶运算 (后半部分)
    for (int len = 2; len <= FFT_SIZE; len <<= 1) {
        int half_len = len >> 1;
        int step = FFT_SIZE / len;

        for (int i = start; i < end; i += len) {
            for (int j = 0; j < half_len; j++) {
                int idx = i + j;
                double complex t = twiddle_factors[j * step] * fft_output[idx + half_len];
                double complex u = fft_output[idx];
                fft_output[idx] = u + t;
                fft_output[idx + half_len] = u - t;
            }
        }

        // 等待 Core 0 完成当前阶段
        multicore_fifo_push_blocking(1);
        multicore_fifo_pop_blocking();
    }

    core_complete[1] = true;
}

// 并行 FFT
void parallel_fft(void) {
    // 初始化输入数据
    for (int i = 0; i < FFT_SIZE; i++) {
        fft_input[i] = sin(2.0 * M_PI * i / FFT_SIZE);
    }

    // 位反转重排序
    int bits = (int)log2(FFT_SIZE);
    for (int i = 0; i < FFT_SIZE; i++) {
        int rev = bit_reverse(i, bits);
        fft_output[rev] = fft_input[i];
    }

    // 启动 Core 1
    multicore_launch_core1(core1_fft_task);

    // Core 0 处理前半部分
    int start = 0;
    int end = FFT_SIZE / NUM_CORES;

    for (int len = 2; len <= FFT_SIZE; len <<= 1) {
        int half_len = len >> 1;
        int step = FFT_SIZE / len;

        for (int i = start; i < end; i += len) {
            for (int j = 0; j < half_len; j++) {
                int idx = i + j;
                double complex t = twiddle_factors[j * step] * fft_output[idx + half_len];
                double complex u = fft_output[idx];
                fft_output[idx] = u + t;
                fft_output[idx + half_len] = u - t;
            }
        }

        // 同步 Core 1
        multicore_fifo_pop_blocking();
        multicore_fifo_push_blocking(1);
    }

    // 等待 Core 1 完成
    while (!core_complete[1]) {
        tight_loop_contents();
    }

    printf("并行 FFT 完成\n");
}

int main(void) {
    stdio_init_all();
    sleep_ms(2000);

    printf("\n=== 多核负载均衡 (FFT) ===\n\n");

    init_twiddle_factors();
    parallel_fft();

    // 打印部分结果
    printf("FFT 结果 (前 8 点):\n");
    for (int i = 0; i < 8; i++) {
        printf("X[%d] = %.4f + %.4fi\n", i,
               creal(fft_output[i]), cimag(fft_output[i]));
    }

    while (1) tight_loop_contents();
}
