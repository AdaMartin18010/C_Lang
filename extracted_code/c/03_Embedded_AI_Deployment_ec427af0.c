/*
 * Auto-generated from: 11_Machine_Learning_C\03_Embedded_AI_Deployment.md
 * Line: 1571
 * Language: c
 * Block ID: ec427af0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file tflm_application.c
 * @brief TFLM完整应用示例 - 关键词唤醒
 *
 * 硬件: STM32F746NG
 * 模型: 20KB MicroSpeech模型
 */

#include <stdio.h>
#include <string.h>
#include "tflm_integration.h"

/* ========== 模型数据 (从Python转换生成) ========== */
/* 实际模型数据应该通过工具生成 */
extern const unsigned char g_model_data[];
extern const unsigned int g_model_len;

/* ========== 配置参数 ========== */
#define TENSOR_ARENA_SIZE     (20 * 1024)   /* 20KB张量内存 */
#define AUDIO_SAMPLE_RATE     16000         /* 16kHz采样率 */
#define AUDIO_WINDOW_MS       30            /* 30ms窗口 */
#define AUDIO_STRIDE_MS       20            /* 20ms步长 */
#define FEATURE_SIZE          40            /* 40维MFCC特征 */
#define SPECTROGRAM_LENGTH    49            /* 时间步数 */

/* ========== 全局变量 ========== */
static uint8_t s_tensor_arena[TENSOR_ARENA_SIZE];
static tflm_context_t s_tflm_ctx;
static tflm_config_t s_tflm_config;

/* 类别标签 */
static const char* kCategoryLabels[] = {
    "silence",    /* 静默 */
    "unknown",    /* 未知 */
    "yes",        /* 是 */
    "no"          /* 否 */
};
#define CATEGORY_COUNT 4

/* ========== 预处理: 音频 -> 频谱图 ========== */

/**
 * @brief 简化的FFT实现 (应使用CMSIS-DSP优化版本)
 */
static void simple_fft(const float* input,
                       float* real_out,
                       float* imag_out,
                       int n)
{
    /* 简化实现 - 实际应用使用CMSIS-DSP arm_rfft_fast_f32 */
    /* 这里仅作占位 */
    (void)input; (void)real_out; (void)imag_out; (void)n;
}

/**
 * @brief 计算MFCC特征
 * @param audio_samples 音频采样 (16kHz, 30ms = 480 samples)
 * @param mfcc_out 输出MFCC特征 (40维)
 */
static void compute_mfcc(const int16_t* audio_samples,
                          float* mfcc_out,
                          int num_mfcc)
{
    /* 1. 预加重 */
    float preemphasized[480];
    const float preemphasis = 0.97f;
    preemphasized[0] = (float)audio_samples[0];
    for (int i = 1; i < 480; i++) {
        preemphasized[i] = (float)audio_samples[i] -
                           preemphasis * (float)audio_samples[i-1];
    }

    /* 2. 加汉明窗 */
    float windowed[480];
    for (int i = 0; i < 480; i++) {
        float window = 0.54f - 0.46f * cosf(2.0f * 3.14159f * i / 479.0f);
        windowed[i] = preemphasized[i] * window;
    }

    /* 3. FFT -> 功率谱 */
    float fft_real[512] = {0};
    float fft_imag[512] = {0};
    memcpy(fft_real, windowed, sizeof(float) * 480);
    simple_fft(fft_real, fft_real, fft_imag, 512);

    /* 计算功率谱 */
    float power_spectrum[256];
    for (int i = 0; i < 256; i++) {
        power_spectrum[i] = fft_real[i] * fft_real[i] +
                            fft_imag[i] * fft_imag[i];
    }

    /* 4. Mel滤波器组 (简化) */
    /* 实际应用应使用预计算的Mel滤波器矩阵 */

    /* 5. Log -> DCT -> MFCC */
    /* 简化为随机值 - 实际应用需要完整的MFCC计算 */
    for (int i = 0; i < num_mfcc && i < 40; i++) {
        mfcc_out[i] = 0.0f;  /* 实际应计算 */
    }
}

/* ========== 主应用程序 ========== */

int main(void)
{
    /* 初始化HAL (平台相关) */
    HAL_Init();
    SystemClock_Config();

    /* 初始化音频采集 (I2S或PDM麦克风) */
    audio_init();

    /* 配置TFLM */
    memset(&s_tflm_config, 0, sizeof(s_tflm_config));
    s_tflm_config.model_data = g_model_data;
    s_tflm_config.model_size = g_model_len;
    s_tflm_config.tensor_arena = s_tensor_arena;
    s_tflm_config.arena_size = TENSOR_ARENA_SIZE;
    s_tflm_config.enable_profiling = 1;

    /* 初始化TFLM */
    int ret = tflm_init(&s_tflm_ctx, &s_tflm_config);
    if (ret != 0) {
        printf("TFLM initialization failed: %d\n", ret);
        Error_Handler();
    }

    printf("KWS System Started\n");
    printf("Listening for 'yes' or 'no'...\n");

    /* 频谱图缓冲区 [时间步 x 特征维度] */
    float spectrogram[SPECTROGRAM_LENGTH][FEATURE_SIZE];
    int spec_index = 0;

    while (1) {
        /* 1. 采集音频 (30ms = 480 samples @ 16kHz) */
        int16_t audio_buffer[480];
        audio_capture(audio_buffer, 480);

        /* 2. 计算MFCC特征 */
        float mfcc[FEATURE_SIZE];
        compute_mfcc(audio_buffer, mfcc, FEATURE_SIZE);

        /* 3. 更新频谱图 (滑动窗口) */
        memcpy(spectrogram[spec_index], mfcc, sizeof(mfcc));
        spec_index = (spec_index + 1) % SPECTROGRAM_LENGTH;

        /* 4. 每收集足够数据执行一次推理 */
        static int frame_count = 0;
        if (++frame_count >= SPECTROGRAM_LENGTH) {
            frame_count = 0;

            /* 准备输入 (转换为INT8) */
            /* 注意: 这里需要根据模型的量化参数进行量化 */
            int8_t quantized_input[SPECTROGRAM_LENGTH * FEATURE_SIZE];

            for (int t = 0; t < SPECTROGRAM_LENGTH; t++) {
                for (int f = 0; f < FEATURE_SIZE; f++) {
                    /* 假设输入scale为0.0390625 (1/25.6) */
                    float scaled = spectrogram[t][f] / 0.0390625f;

                    if (scaled > 127.0f) scaled = 127.0f;
                    if (scaled < -128.0f) scaled = -128.0f;

                    quantized_input[t * FEATURE_SIZE + f] =
                        (int8_t)(scaled > 0 ? scaled + 0.5f : scaled - 0.5f);
                }
            }

            /* 5. 执行推理 */
            uint32_t start = HAL_GetTick();

            tflm_prepare_input(&s_tflm_ctx, quantized_input,
                               sizeof(quantized_input));
            tflm_invoke(&s_tflm_ctx);

            uint32_t inference_time = HAL_GetTick() - start;

            /* 6. 获取输出 */
            int8_t output[CATEGORY_COUNT];
            tflm_get_output(&s_tflm_ctx, output, sizeof(output));

            /* 7. 后处理: 去量化并找到最大概率 */
            float probabilities[CATEGORY_COUNT];
            float max_prob = -128.0f;
            int max_index = 0;

            /* 假设输出scale为0.00390625 (1/256) */
            const float output_scale = 0.00390625f;

            for (int i = 0; i < CATEGORY_COUNT; i++) {
                probabilities[i] = output[i] * output_scale;
                if (output[i] > max_prob) {
                    max_prob = output[i];
                    max_index = i;
                }
            }

            /* 8. 应用检测阈值 */
            const float detection_threshold = 0.7f;
            float dequantized_max = max_prob * output_scale;

            if (dequantized_max > detection_threshold &&
                max_index >= 2) {  /* yes/no */
                printf("Detected: %s (confidence: %.2f, time: %lu ms)\n",
                       kCategoryLabels[max_index],
                       dequantized_max,
                       inference_time);

                /* 触发相应动作 */
                if (max_index == 2) {  /* yes */
                    LED_On(LED_GREEN);
                } else {  /* no */
                    LED_On(LED_RED);
                }
            }
        }

        /* 等待下一个音频窗口 (20ms步长) */
        HAL_Delay(20);
    }
}

/* ========== 辅助函数 ========== */

void Error_Handler(void)
{
    /* 错误处理: 闪烁LED */
    while (1) {
        LED_Toggle(LED_RED);
        HAL_Delay(200);
    }
}

void audio_init(void)
{
    /* I2S/PDM初始化 */
}

void audio_capture(int16_t* buffer, uint32_t samples)
{
    /* 从DMA缓冲区读取音频数据 */
}

void LED_On(int led_id) { /* ... */ }
void LED_Toggle(int led_id) { /* ... */ }
