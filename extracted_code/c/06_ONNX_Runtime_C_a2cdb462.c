/*
 * Auto-generated from: 11_Machine_Learning_C\06_ONNX_Runtime_C.md
 * Line: 448
 * Language: c
 * Block ID: a2cdb462
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 处理动态批次大小
int64_t input_dims[] = {-1, 3, 224, 224};  // -1表示动态

// 使用符号维度
OrtSessionOptions* options;
g_ort->CreateSessionOptions(&options);
g_ort->AddFreeDimensionOverrideByName(options, "batch_size", 1);
