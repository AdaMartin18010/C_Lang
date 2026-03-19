/*
 * Auto-generated from: 11_Machine_Learning_C\07_llama_cpp_Deployment.md
 * Line: 225
 * Language: c
 * Block ID: a15f44ee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// llama_integration.c
#include "llama_integration.h"
#include <string.h>
#include <stdio.h>

bool llama_engine_init(llama_engine_t* engine,
                       const char* model_path,
                       int n_gpu_layers,
                       int n_ctx) {
    // 模型参数
    struct llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = n_gpu_layers;  // GPU层数

    // 加载模型
    engine->model = llama_load_model_from_file(model_path, model_params);
    if (!engine->model) {
        fprintf(stderr, "Failed to load model: %s\n", model_path);
        return false;
    }

    // 上下文参数
    struct llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = n_ctx;           // 上下文长度
    ctx_params.n_batch = 512;           // 批处理大小
    ctx_params.n_threads = 4;           // CPU线程数
    ctx_params.n_threads_batch = 4;     // 批处理线程数

    // 创建上下文
    engine->ctx = llama_new_context_with_model(engine->model, ctx_params);
    if (!engine->ctx) {
        fprintf(stderr, "Failed to create context\n");
        llama_free_model(engine->model);
        return false;
    }

    // 创建采样器
    engine->sampler = llama_sampler_chain_init(llama_sampler_chain_default_params());
    llama_sampler_chain_add(engine->sampler, llama_sampler_init_top_k(40));
    llama_sampler_chain_add(engine->sampler, llama_sampler_init_top_p(0.95, 1));
    llama_sampler_chain_add(engine->sampler, llama_sampler_init_temp(0.8));
    llama_sampler_chain_add(engine->sampler, llama_sampler_init_dist(12345));

    engine->n_ctx = n_ctx;
    engine->n_batch = 512;

    return true;
}

int llama_engine_generate(llama_engine_t* engine,
                          const char* prompt,
                          char* output,
                          int output_size,
                          int max_tokens) {
    // Tokenize prompt
    const int n_prompt_tokens = -llama_tokenize(
        engine->model,
        prompt,
        strlen(prompt),
        NULL,
        0,
        true,
        true
    );

    llama_token* prompt_tokens = malloc(n_prompt_tokens * sizeof(llama_token));
    llama_tokenize(
        engine->model,
        prompt,
        strlen(prompt),
        prompt_tokens,
        n_prompt_tokens,
        true,
        true
    );

    // 评估prompt
    llama_batch batch = llama_batch_init(engine->n_batch, 0, 1);

    for (int i = 0; i < n_prompt_tokens; i++) {
        llama_batch_add(batch, prompt_tokens[i], i, NULL, false);
    }
    batch.logits[batch.n_tokens - 1] = true;  // 最后一个token需要logits

    if (llama_decode(engine->ctx, batch) != 0) {
        fprintf(stderr, "Failed to decode\n");
        free(prompt_tokens);
        return -1;
    }

    // 生成tokens
    int n_generated = 0;
    int output_pos = 0;

    while (n_generated < max_tokens && output_pos < output_size - 1) {
        // 采样下一个token
        llama_token new_token_id = llama_sampler_sample(
            engine->sampler,
            engine->ctx,
            batch.n_tokens - 1
        );

        // 检查结束条件
        if (llama_token_is_eog(engine->model, new_token_id)) {
            break;
        }

        // 转换为文本
        char piece[256];
        int n_chars = llama_token_to_piece(
            engine->model,
            new_token_id,
            piece,
            sizeof(piece),
            0,
            true
        );

        if (output_pos + n_chars < output_size) {
            memcpy(output + output_pos, piece, n_chars);
            output_pos += n_chars;
            output[output_pos] = '\0';
        }

        // 准备下一次迭代
        llama_batch_clear(batch);
        llama_batch_add(batch, new_token_id, n_prompt_tokens + n_generated, NULL, true);

        if (llama_decode(engine->ctx, batch) != 0) {
            break;
        }

        n_generated++;
    }

    // 清理
    llama_batch_free(batch);
    free(prompt_tokens);

    return n_generated;
}

void llama_engine_free(llama_engine_t* engine) {
    if (engine->sampler) {
        llama_sampler_free(engine->sampler);
        engine->sampler = NULL;
    }
    if (engine->ctx) {
        llama_free(engine->ctx);
        engine->ctx = NULL;
    }
    if (engine->model) {
        llama_free_model(engine->model);
        engine->model = NULL;
    }
}
