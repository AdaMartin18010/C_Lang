/*
 * Auto-generated from: 10_WebAssembly_C\04_WASM_Performance_Optimization.md
 * Line: 480
 * Language: c
 * Block ID: b7dde111
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// simd_applications.c
#include <wasm_simd128.h>
#include <emscripten/emscripten.h>
#include <math.h>

// ========== 数组求和 ==========

EMSCRIPTEN_KEEPALIVE
float sum_array_simd(const float* arr, int n) {
    v128_t sum4 = wasm_f32x4_splat(0.0f);
    int i = 0;

    // 每次处理 4 个元素
    for (; i + 4 <= n; i += 4) {
        v128_t vec = wasm_v128_load(&arr[i]);
        sum4 = wasm_f32x4_add(sum4, vec);
    }

    // 水平求和
    // sum4 = [a, b, c, d]
    // 需要: a + b + c + d
    v128_t shuffled = wasm_i32x4_shuffle(sum4, sum4, 2, 3, 0, 1);  // [c, d, a, b]
    v128_t sum2 = wasm_f32x4_add(sum4, shuffled);                   // [a+c, b+d, ...]
    shuffled = wasm_i32x4_shuffle(sum2, sum2, 1, 0, 3, 2);          // [b+d, a+c, ...]
    v128_t sum1 = wasm_f32x4_add(sum2, shuffled);                   // [sum, sum, ...]

    float result = wasm_f32x4_extract_lane(sum1, 0);

    // 处理剩余元素
    for (; i < n; i++) {
        result += arr[i];
    }

    return result;
}

// ========== 点积 ==========

EMSCRIPTEN_KEEPALIVE
float dot_product_simd(const float* a, const float* b, int n) {
    v128_t sum = wasm_f32x4_splat(0.0f);
    int i = 0;

    for (; i + 4 <= n; i += 4) {
        v128_t va = wasm_v128_load(&a[i]);
        v128_t vb = wasm_v128_load(&b[i]);
        sum = wasm_f32x4_add(sum, wasm_f32x4_mul(va, vb));
    }

    // 水平求和
    float result = 0;
    for (int j = 0; j < 4; j++) {
        result += wasm_f32x4_extract_lane(sum, j);
    }

    for (; i < n; i++) {
        result += a[i] * b[i];
    }

    return result;
}

// ========== 图像处理：RGBA 转灰度 ==========

EMSCRIPTEN_KEEPALIVE
void rgba_to_grayscale_simd(const unsigned char* rgba, unsigned char* gray, int num_pixels) {
    // 灰度系数
    const v128_t r_coeff = wasm_f32x4_splat(0.299f);
    const v128_t g_coeff = wasm_f32x4_splat(0.587f);
    const v128_t b_coeff = wasm_f32x4_splat(0.114f);

    int i = 0;

    // 每次处理 4 个像素 (16 字节 = 1 SIMD 寄存器)
    for (; i + 4 <= num_pixels; i += 4) {
        // 加载 4 个 RGBA 像素
        v128_t pixels = wasm_v128_load(rgba + i * 4);

        // 扩展为 16 位（用于分离通道）
        v128_t pixel16_lo = wasm_i16x8_extend_low_i8x16(pixels);
        v128_t pixel16_hi = wasm_i16x8_extend_high_i8x16(pixels);

        // 转换为 float 并计算（简化版）
        // 实际实现需要更复杂的通道分离...

        // 存储结果
        // wasm_v128_store(gray + i, result);
    }

    // 处理剩余像素
    for (; i < num_pixels; i++) {
        int r = rgba[i * 4];
        int g = rgba[i * 4 + 1];
        int b = rgba[i * 4 + 2];
        gray[i] = (unsigned char)(0.299f * r + 0.587f * g + 0.114f * b);
    }
}

// ========== 矩阵乘法（4x4） ==========

EMSCRIPTEN_KEEPALIVE
void matrix4x4_multiply_simd(const float* a, const float* b, float* result) {
    // 加载 B 的列
    v128_t b_col0 = wasm_v128_load(b);
    v128_t b_col1 = wasm_v128_load(b + 4);
    v128_t b_col2 = wasm_v128_load(b + 8);
    v128_t b_col3 = wasm_v128_load(b + 12);

    for (int i = 0; i < 4; i++) {
        // 加载 A 的行并广播到每个元素
        v128_t a_row = wasm_v128_load(a + i * 4);

        v128_t a0 = wasm_i32x4_shuffle(a_row, a_row, 0, 0, 0, 0);
        v128_t a1 = wasm_i32x4_shuffle(a_row, a_row, 1, 1, 1, 1);
        v128_t a2 = wasm_i32x4_shuffle(a_row, a_row, 2, 2, 2, 2);
        v128_t a3 = wasm_i32x4_shuffle(a_row, a_row, 3, 3, 3, 3);

        // 计算结果行
        v128_t row = wasm_f32x4_add(
            wasm_f32x4_mul(a0, b_col0),
            wasm_f32x4_add(
                wasm_f32x4_mul(a1, b_col1),
                wasm_f32x4_add(
                    wasm_f32x4_mul(a2, b_col2),
                    wasm_f32x4_mul(a3, b_col3)
                )
            )
        );

        wasm_v128_store(result + i * 4, row);
    }
}

// ========== 音频处理：淡入淡出 ==========

EMSCRIPTEN_KEEPALIVE
void apply_fade_simd(float* audio, int samples, float start_gain, float end_gain) {
    const float step = (end_gain - start_gain) / samples;

    v128_t vstep = wasm_f32x4_splat(step * 4);
    v128_t vgain = wasm_f32x4_make(start_gain,
                                    start_gain + step,
                                    start_gain + step * 2,
                                    start_gain + step * 3);

    int i = 0;
    for (; i + 4 <= samples; i += 4) {
        v128_t sample = wasm_v128_load(&audio[i]);
        sample = wasm_f32x4_mul(sample, vgain);
        wasm_v128_store(&audio[i], sample);

        vgain = wasm_f32x4_add(vgain, vstep);
    }

    // 处理剩余
    float gain = start_gain + i * step;
    for (; i < samples; i++) {
        audio[i] *= gain;
        gain += step;
    }
}
