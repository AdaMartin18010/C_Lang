/*
 * Auto-generated from: 11_Machine_Learning_C\06_ONNX_Runtime_C.md
 * Line: 131
 * Language: c
 * Block ID: b88c6a5a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    OrtEnv* env;
    OrtSessionOptions* session_options;
    OrtSession* session;
    OrtMemoryInfo* memory_info;
} ONNX_Runtime;

ONNX_Runtime* onnx_create_runtime(const char* model_path) {
    ONNX_Runtime* rt = calloc(1, sizeof(ONNX_Runtime));
    OrtStatus* status = NULL;

    // 创建环境
    status = g_ort->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "inference", &rt->env);
    CHECK_STATUS(status);

    // 会话选项
    status = g_ort->CreateSessionOptions(&rt->session_options);
    CHECK_STATUS(status);

    // 设置线程数
    g_ort->SetIntraOpNumThreads(rt->session_options, 4);
    g_ort->SetInterOpNumThreads(rt->session_options, 4);

    // 图优化
    g_ort->SetSessionGraphOptimizationLevel(
        rt->session_options,
        ORT_ENABLE_ALL);

    // 创建会话
    status = g_ort->CreateSession(rt->env, model_path,
                                   rt->session_options, &rt->session);
    CHECK_STATUS(status);

    // CPU内存信息
    status = g_ort->CreateCpuMemoryInfo(
        OrtArenaAllocator, OrtMemTypeDefault, &rt->memory_info);
    CHECK_STATUS(status);

    return rt;
}

void onnx_destroy_runtime(ONNX_Runtime* rt) {
    if (!rt) return;
    g_ort->ReleaseMemoryInfo(rt->memory_info);
    g_ort->ReleaseSession(rt->session);
    g_ort->ReleaseSessionOptions(rt->session_options);
    g_ort->ReleaseEnv(rt->env);
    free(rt);
}
