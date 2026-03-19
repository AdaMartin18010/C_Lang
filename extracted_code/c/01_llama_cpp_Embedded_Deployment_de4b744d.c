/*
 * Auto-generated from: 16_Embedded_AI_Agents\02_LLMs_on_Edge\01_llama_cpp_Embedded_Deployment.md
 * Line: 192
 * Language: c
 * Block ID: de4b744d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// llama_integration.c
#include "llama.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    struct llama_model* model;
    struct llama_context* ctx;
    const struct llama_vocab* vocab;
} llama_wrapper_t;

// 初始化模型
llama_wrapper_t* llama_init(const char* model_path, int n_ctx) {
    llama_wrapper_t* wrapper = calloc(1, sizeof(llama_wrapper_t));

    // 模型参数
    struct llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = 0;  // CPU only for embedded

    // 加载模型
    wrapper->model = llama_load_model_from_file(model_path, model_params);
    if (!wrapper->model) {
        fprintf(stderr, "Failed to load model\n");
        free(wrapper);
        return NULL;
    }

    // 上下文参数
    struct llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = n_ctx;  // 上下文长度
    ctx_params.n_threads = 4;   // CPU线程数
    ctx_params.n_threads_batch = 4;

    // 创建上下文
    wrapper->ctx = llama_new_context_with_model(wrapper->model, ctx_params);
    if (!wrapper->ctx) {
        fprintf(stderr, "Failed to create context\n");
        llama_free_model(wrapper->model);
        free(wrapper);
        return NULL;
    }

    wrapper->vocab = llama_get_vocab(wrapper->model);

    printf("Model loaded: n_vocab=%d, n_ctx=%d\n",
           llama_vocab_n_tokens(wrapper->vocab),
           llama_n_ctx(wrapper->ctx));

    return wrapper;
}

// 生成文本
int llama_generate(llama_wrapper_t* wrapper,
                   const char* prompt,
                   char* output,
                   int max_tokens,
                   float temperature) {

    // Tokenize输入
    const int n_prompt_tokens = max_tokens + 256;
    llama_token* prompt_tokens = malloc(n_prompt_tokens * sizeof(llama_token));

    int n_tokens = llama_tokenize(
        wrapper->vocab,
        prompt,
        strlen(prompt),
        prompt_tokens,
        n_prompt_tokens,
        true,   // add special tokens
        true    // parse special
    );

    if (n_tokens < 0) {
        fprintf(stderr, "Tokenization failed\n");
        free(prompt_tokens);
        return -1;
    }

    // 创建batch
    struct llama_batch batch = llama_batch_init(n_tokens, 0, 1);

    for (int i = 0; i < n_tokens; i++) {
        llama_batch_add(batch, prompt_tokens[i], i, {0}, false);
    }
    batch.logits[batch.n_tokens - 1] = 1;  // 最后一个token需要logits

    // 解码
    if (llama_decode(wrapper->ctx, batch) != 0) {
        fprintf(stderr, "Decode failed\n");
        llama_batch_free(batch);
        free(prompt_tokens);
        return -1;
    }

    // 生成循环
    int n_cur = batch.n_tokens;
    int n_gen = 0;
    llama_token* generated = malloc(max_tokens * sizeof(llama_token));

    while (n_cur < max_tokens && n_gen < max_tokens) {
        // 采样下一个token
        llama_token new_token_id = llama_sample_token_greedy(
            wrapper->ctx,
            llama_get_logits_ith(wrapper->ctx, batch.n_tokens - 1)
        );

        // 检查结束符
        if (llama_vocab_is_eog(wrapper->vocab, new_token_id)) {
            break;
        }

        generated[n_gen++] = new_token_id;

        // 准备下一个batch
        llama_batch_clear(batch);
        llama_batch_add(batch, new_token_id, n_cur++, {0}, true);

        if (llama_decode(wrapper->ctx, batch) != 0) {
            break;
        }
    }

    // 转换token为文本
    int output_len = 0;
    for (int i = 0; i < n_gen && output_len < max_tokens - 1; i++) {
        char piece[256];
        int n = llama_token_to_piece(
            wrapper->vocab,
            generated[i],
            piece,
            sizeof(piece),
            0,  // special
            true // with space
        );
        if (n > 0 && output_len + n < max_tokens - 1) {
            memcpy(output + output_len, piece, n);
            output_len += n;
        }
    }
    output[output_len] = '\0';

    // 清理
    llama_batch_free(batch);
    free(prompt_tokens);
    free(generated);

    return n_gen;
}

// 清理资源
void llama_cleanup(llama_wrapper_t* wrapper) {
    if (wrapper) {
        llama_free(wrapper->ctx);
        llama_free_model(wrapper->model);
        free(wrapper);
    }
}

// 主函数示例
int main() {
    const char* model_path = "/home/pi/models/Llama-3.2-3B-Instruct-Q4_K_M.gguf";

    printf("Loading model...\n");
    llama_wrapper_t* llama = llama_init(model_path, 2048);
    if (!llama) {
        return 1;
    }

    const char* prompt = "User: What is TinyML?\nAssistant:";
    char response[1024];

    printf("Prompt: %s\n", prompt);
    printf("Generating...\n");

    int n_gen = llama_generate(llama, prompt, response, 512, 0.7f);

    printf("Response (%d tokens): %s\n", n_gen, response);

    llama_cleanup(llama);
    return 0;
}
