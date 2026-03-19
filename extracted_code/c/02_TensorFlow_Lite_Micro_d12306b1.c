/*
 * Auto-generated from: 16_Embedded_AI_Agents\01_TinyML_Fundamentals\02_TensorFlow_Lite_Micro.md
 * Line: 475
 * Language: c
 * Block ID: d12306b1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 内存优化配置 */
#define TENSOR_ARENA_SIZE   (16 * 1024)   /* 最小化内存 */

/* 使用内存规划器优化 */
#include "tensorflow/lite/micro/memory_planner/greedy_memory_planner.h"

/* 分析内存使用 */
void analyze_memory_usage(void) {
    size_t used = tflm_get_used_memory();
    size_t total = TENSOR_ARENA_SIZE;

    printf("Memory Usage: %zu / %zu bytes (%.1f%%)\r\n",
           used, total, (float)used / total * 100);
}
