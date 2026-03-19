/*
 * Auto-generated from: 11_Machine_Learning_C\07_llama_cpp_Deployment.md
 * Line: 389
 * Language: c
 * Block ID: 2812936c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 内存映射加载 (减少RAM占用)
struct llama_model_params model_params = llama_model_default_params();
model_params.use_mmap = true;   // 使用内存映射
model_params.use_mlock = false; // 不锁定内存 (允许交换)

// KV缓存优化
struct llama_context_params ctx_params = llama_context_default_params();
ctx_params.n_ctx = 4096;        // 根据需求调整
ctx_params.n_batch = 512;       // 批处理大小
ctx_params.flash_attn = true;   // Flash Attention (节省内存)
