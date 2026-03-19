/*
 * Auto-generated from: 16_Embedded_AI_Agents\02_LLMs_on_Edge\02_GGML_Optimization.md
 * Line: 539
 * Language: c
 * Block ID: 8a8cedcc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* main.cpp - 简单命令行聊天应用 */

#include "llama.h"
#include <iostream>
#include <string>
#include <vector>

class EdgeLLM {
public:
    EdgeLLM(const std::string& model_path, int n_ctx = 2048) {
        // 模型参数
        llama_model_params model_params = llama_model_default_params();
        model_params.use_mmap = true;     // 启用内存映射
        model_params.use_mlock = false;   // 不锁定内存

        // 加载模型
        model_ = llama_load_model_from_file(model_path.c_str(), model_params);
        if (!model_) {
            throw std::runtime_error("Failed to load model");
        }

        // 上下文参数
        llama_context_params ctx_params = llama_context_default_params();
        ctx_params.n_ctx = n_ctx;
        ctx_params.n_batch = 512;
        ctx_params.n_threads = 4;         // 根据CPU调整
        ctx_params.n_threads_batch = 4;

        // 创建上下文
        ctx_ = llama_new_context_with_model(model_, ctx_params);
        if (!ctx_) {
            llama_free_model(model_);
            throw std::runtime_error("Failed to create context");
        }

        // 初始化采样器
        sparams_ = llama_sampler_chain_default_params();
        sparams_.no_perf = false;
        sampler_ = llama_sampler_chain_init(sparams_);

        // 添加采样参数
        llama_sampler_chain_add(sampler_,
            llama_sampler_init_temp(0.8f));        // 温度
        llama_sampler_chain_add(sampler_,
            llama_sampler_init_top_p(0.95f, 1));   // Top-p
        llama_sampler_chain_add(sampler_,
            llama_sampler_init_top_k(40, 1));      // Top-k
    }

    ~EdgeLLM() {
        llama_sampler_free(sampler_);
        llama_free(ctx_);
        llama_free_model(model_);
    }

    std::string generate(const std::string& prompt, int max_tokens = 256) {
        // Tokenize
        const int n_prompt_tokens = -llama_tokenize(
            model_, prompt.c_str(), prompt.length(),
            nullptr, 0, true, true
        );

        std::vector<llama_token> prompt_tokens(n_prompt_tokens);
        llama_tokenize(model_, prompt.c_str(), prompt.length(),
                      prompt_tokens.data(), prompt_tokens.size(),
                      true, true);

        // 评估提示
        llama_batch batch = llama_batch_init(n_prompt_tokens, 0, 1);
        for (size_t i = 0; i < prompt_tokens.size(); i++) {
            batch.token[i] = prompt_tokens[i];
            batch.pos[i] = i;
            batch.n_seq_id[i] = 1;
            batch.seq_id[i][0] = 0;
            batch.logits[i] = 0;
        }
        batch.logits[n_prompt_tokens - 1] = 1;
        batch.n_tokens = n_prompt_tokens;

        llama_decode(ctx_, batch);
        llama_batch_free(batch);

        // 生成响应
        std::string response;
        int n_gen = 0;

        while (n_gen < max_tokens) {
            // 采样
            llama_token new_token = llama_sampler_sample(sampler_, ctx_, -1);

            // 检查结束
            if (llama_token_is_eog(model_, new_token)) {
                break;
            }

            // 转换为文本
            char buf[32];
            int n = llama_token_to_piece(model_, new_token, buf, sizeof(buf),
                                        0, true);
            response.append(buf, n);

            // 打印实时输出
            std::cout.write(buf, n);
            std::cout.flush();

            // 下一次迭代
            llama_batch single = llama_batch_init(1, 0, 1);
            single.token[0] = new_token;
            single.pos[0] = n_prompt_tokens + n_gen;
            single.n_seq_id[0] = 1;
            single.seq_id[0][0] = 0;
            single.logits[0] = 1;
            single.n_tokens = 1;

            llama_decode(ctx_, single);
            llama_batch_free(single);

            n_gen++;
        }

        return response;
    }

private:
    llama_model* model_;
    llama_context* ctx_;
    llama_sampler* sampler_;
    llama_sampler_chain_params sparams_;
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <model.gguf>\n";
        return 1;
    }

    try {
        std::cout << "Loading model: " << argv[1] << "\n";
        EdgeLLM llm(argv[1]);
        std::cout << "Model loaded. Ready for chat.\n\n";

        std::string input;
        while (true) {
            std::cout << "\nYou: ";
            std::getline(std::cin, input);

            if (input == "quit" || input == "exit") {
                break;
            }

            std::cout << "\nAssistant: ";
            llm.generate(input);
            std::cout << "\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
