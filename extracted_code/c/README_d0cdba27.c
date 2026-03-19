/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 1068
 * Language: c
 * Block ID: d0cdba27
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 异步操作回调
typedef void (*SuccessCallback)(void* result, void* user_data);
typedef void (*ErrorCallback)(int error_code, const char* msg, void* user_data);

typedef struct {
    SuccessCallback on_success;
    ErrorCallback on_error;
    void* user_data;
} AsyncCallbacks;

void async_operation(const char* request, const AsyncCallbacks* cb);

// 使用示例
void on_data_received(void* result, void* user_data) {
    HttpResponse* resp = result;
    int request_id = *(int*)user_data;
    printf("Request %d completed: %s\n", request_id, resp->body);
}

void on_error(int code, const char* msg, void* user_data) {
    int request_id = *(int*)user_data;
    printf("Request %d failed [%d]: %s\n", request_id, code, msg);
}

void fetch_data(void) {
    static int request_id = 1;
    AsyncCallbacks cb = {
        .on_success = on_data_received,
        .on_error = on_error,
        .user_data = &request_id
    };
    async_operation("https://api.example.com/data", &cb);
    request_id++;
}
