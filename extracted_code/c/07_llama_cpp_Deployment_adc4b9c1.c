/*
 * Auto-generated from: 11_Machine_Learning_C\07_llama_cpp_Deployment.md
 * Line: 190
 * Language: c
 * Block ID: adc4b9c1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// llama_integration.h
#ifndef LLAMA_INTEGRATION_H
#define LLAMA_INTEGRATION_H

#include <llama.h>
#include <stdbool.h>

typedef struct {
    llama_model* model;
    llama_context* ctx;
    llama_sampler* sampler;
    int n_ctx;
    int n_batch;
} llama_engine_t;

// 初始化引擎
bool llama_engine_init(llama_engine_t* engine,
                       const char* model_path,
                       int n_gpu_layers,
                       int n_ctx);

// 生成文本
int llama_engine_generate(llama_engine_t* engine,
                          const char* prompt,
                          char* output,
                          int output_size,
                          int max_tokens);

// 释放资源
void llama_engine_free(llama_engine_t* engine);

#endif
