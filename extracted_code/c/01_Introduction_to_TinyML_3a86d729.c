/*
 * Auto-generated from: 16_Embedded_AI_Agents\01_TinyML_Fundamentals\01_Introduction_to_TinyML.md
 * Line: 417
 * Language: c
 * Block ID: 3a86d729
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用静态分配避免堆碎片
static uint8_t tensor_arena[10 * 1024] __attribute__((aligned(16)));
static TfLiteTensor input_buffer;

// 避免动态内存
// ❌ 不推荐
float* buffer = malloc(size);

// ✅ 推荐
static float buffer[MAX_SIZE];
