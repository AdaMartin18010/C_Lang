/*
 * Auto-generated from: 16_Embedded_AI_Agents\02_LLMs_on_Edge\02_GGML_Optimization.md
 * Line: 373
 * Language: c
 * Block ID: f1d5a2e2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 低内存推理实现 - 适用于RAM < 4GB的设备 */

#include "llama.h"
#include <stdio.h>
#include <string.h>

/* 内存监控 */
static size_t max_memory_used = 0;

void memory_callback(void* user_data, enum ggml_op op,
                     struct ggml_tensor* tensor, bool ingrad) {
    size_t current = ggml_used_mem((struct ggml_context*)user_data);
    if (current > max_memory_used) {
        max_memory_used = current;
    }
}

/* 流式推理 - 逐token生成，降低峰值内存 */
int stream_generate(llama_context* ctx, const char* prompt,
                    char* output, size_t output_size) {

    // 1. Tokenize输入
    const int n_prompt_tokens = -llama_tokenize(
        llama_get_model(ctx),
        prompt,
        strlen(prompt),
        NULL,
        0,
        true,
        true
    );

    llama_token* prompt_tokens = malloc(n_prompt_tokens * sizeof(llama_token));
    llama_tokenize(
        llama_get_model(ctx),
        prompt,
        strlen(prompt),
        prompt_tokens,
        n_prompt_tokens,
        true,
        true
    );

    // 2. 评估提示 (单次批处理)
    llama_batch batch = llama_batch_init(n_prompt_tokens, 0, 1);

    for (int i = 0; i < n_prompt_tokens; i++) {
        batch.token[i] = prompt_tokens[i];
        batch.pos[i] = i;
        batch.n_seq_id[i] = 1;
        batch.seq_id[i][0] = 0;
        batch.logits[i] = 0;
    }
    batch.logits[n_prompt_tokens - 1] = 1;  // 最后一个token计算logits
    batch.n_tokens = n_prompt_tokens;

    llama_decode(ctx, batch);
    llama_batch_free(batch);

    // 3. 逐个生成token (流式)
    int n_gen = 0;
    int max_tokens = output_size / 4;  // 估计最大token数
    llama_token new_token_id;

    while (n_gen < max_tokens) {
        // 采样下一个token
        new_token_id = llama_sample_token_greedy(
            ctx,
            llama_get_logits_ith(ctx, -1)
        );

        // 检查结束符
        if (llama_token_is_eog(llama_get_model(ctx), new_token_id)) {
            break;
        }

        // 转换为文本并输出
        char token_str[32];
        int n_str = llama_token_to_piece(
            llama_get_model(ctx),
            new_token_id,
            token_str,
            sizeof(token_str),
            0,
            true
        );

        // 追加到输出
        strncat(output, token_str, output_size - strlen(output) - 1);

        // 准备下一个输入 (单个token)
        llama_batch single = llama_batch_init(1, 0, 1);
        single.token[0] = new_token_id;
        single.pos[0] = n_prompt_tokens + n_gen;
        single.n_seq_id[0] = 1;
        single.seq_id[0][0] = 0;
        single.logits[0] = 1;
        single.n_tokens = 1;

        llama_decode(ctx, single);
        llama_batch_free(single);

        n_gen++;

        // 内存检查
        if (n_gen % 10 == 0) {
            printf("Generated %d tokens, peak mem: %.1f MB\n",
                   n_gen, max_memory_used / (1024.0 * 1024.0));
        }
    }

    free(prompt_tokens);
    return n_gen;
}
