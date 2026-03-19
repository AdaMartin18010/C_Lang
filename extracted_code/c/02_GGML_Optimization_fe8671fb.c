/*
 * Auto-generated from: 16_Embedded_AI_Agents\02_LLMs_on_Edge\02_GGML_Optimization.md
 * Line: 339
 * Language: c
 * Block ID: fe8671fb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* llama.cpp 内存优化配置 */

// llama_context_params 配置
struct llama_context_params params = llama_context_default_params();

// 1. 启用内存映射 (mmap)
params.use_mmap = true;      // 使用mmap加载模型
params.use_mlock = false;    // 不锁定内存(嵌入式通常不支持)

// 2. 限制上下文长度
params.n_ctx = 512;          // 上下文长度 (默认4096，减小可省内存)
params.n_batch = 128;        // 批处理大小

// 3. 线程配置
params.n_threads = 2;        // 根据CPU核心数调整
params.n_threads_batch = 2;  // 批处理线程

// 4. 量化配置 (GGUF加载时)
llama_model_params model_params = llama_model_default_params();
model_params.use_mmap = true;
model_params.use_mlock = false;

// 加载模型
llama_model* model = llama_load_model_from_file(
    "model-q4_0.gguf",
    model_params
);

llama_context* ctx = llama_new_context_with_model(model, params);
