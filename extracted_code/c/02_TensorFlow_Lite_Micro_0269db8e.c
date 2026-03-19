/*
 * Auto-generated from: 16_Embedded_AI_Agents\01_TinyML_Fundamentals\02_TensorFlow_Lite_Micro.md
 * Line: 245
 * Language: c
 * Block ID: 0269db8e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* TensorFlow Lite Micro 核心API */
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

/* 模型数据 (由转换脚本生成) */
extern const uint8_t g_model[];
extern const unsigned int g_model_len;

/* 运行时配置 */
#define TENSOR_ARENA_SIZE   (32 * 1024)   /* 32KB张量内存 */
