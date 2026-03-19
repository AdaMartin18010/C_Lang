/*
 * Auto-generated from: 11_Machine_Learning_C\07_llama_cpp_Deployment.md
 * Line: 449
 * Language: c
 * Block ID: 27b5cf73
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单的HTTP服务器示例 (使用llama.cpp server)
// 实际生产建议使用 llama.cpp/examples/server

#include <microhttpd.h>  // libmicrohttpd

typedef struct {
    llama_engine_t engine;
    pthread_mutex_t lock;
    int max_concurrent;
    int active_requests;
} llama_server_t;

// 请求处理
static enum MHD_Result request_handler(void* cls,
                                        struct MHD_Connection* connection,
                                        const char* url,
                                        const char* method,
                                        const char* version,
                                        const char* upload_data,
                                        size_t* upload_data_size,
                                        void** con_cls) {
    llama_server_t* server = (llama_server_t*)cls;

    // 限流检查
    pthread_mutex_lock(&server->lock);
    if (server->active_requests >= server->max_concurrent) {
        pthread_mutex_unlock(&server->lock);
        return send_error_response(connection, 503, "Server busy");
    }
    server->active_requests++;
    pthread_mutex_unlock(&server->lock);

    // 解析请求
    const char* prompt = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "prompt");
    if (!prompt) {
        server->active_requests--;
        return send_error_response(connection, 400, "Missing prompt");
    }

    // 生成响应
    char output[8192];
    pthread_mutex_lock(&server->lock);  // llama_engine非线程安全
    llama_engine_generate(&server->engine, prompt, output, sizeof(output), 512);
    pthread_mutex_unlock(&server->lock);

    server->active_requests--;

    // 发送响应
    return send_json_response(connection, output);
}
