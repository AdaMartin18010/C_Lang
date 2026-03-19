/*
 * Auto-generated from: 11_Machine_Learning_C\07_llama_cpp_Deployment.md
 * Line: 404
 * Language: c
 * Block ID: 67a9b82a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// CPU优化
ctx_params.n_threads = omp_get_max_threads();  // 使用所有核心
ctx_params.n_threads_batch = omp_get_max_threads();

// GPU卸载
model_params.n_gpu_layers = 999;  // 所有层放GPU

// 混合精度
ctx_params.type_k = GGML_TYPE_F16;  // Key cache FP16
ctx_params.type_v = GGML_TYPE_F16;  // Value cache FP16
