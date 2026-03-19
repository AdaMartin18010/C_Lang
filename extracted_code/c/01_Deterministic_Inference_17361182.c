/*
 * Auto-generated from: 16_Embedded_AI_Agents\08_Real_Time_AI\01_Deterministic_Inference.md
 * Line: 61
 * Language: c
 * Block ID: 17361182
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 传统方式 - 动态内存，非确定性
float* buffer = malloc(layer_size * sizeof(float));
if (!buffer) return ERROR;
layer_compute(input, buffer);
free(buffer);

// ✅ 实时方式 - 静态内存池
typedef struct {
    float buffer_0[256 * 256];  // Layer 0 output
    float buffer_1[128 * 128];  // Layer 1 output
    float buffer_2[64 * 64];    // Layer 2 output
    // ... 所有层预分配
} inference_buffers_t;

static inference_buffers_t g_buffers;  // 全局静态分配

int deterministic_inference(const float* input, float* output) {
    // 无malloc/free，执行时间确定
    layer_0_compute(input, g_buffers.buffer_0);
    layer_1_compute(g_buffers.buffer_0, g_buffers.buffer_1);
    layer_2_compute(g_buffers.buffer_1, output);
    return 0;
}
