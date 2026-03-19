/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\03_Storage_Duration_Matrix.md
 * Line: 195
 * Language: c
 * Block ID: cbf4ab24
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 自动存储期 - 错误码返回
int divide_auto(int a, int b, int* result) {
    if (b == 0) return -1;  // 错误码
    *result = a / b;
    return 0;
}

// 静态存储期 - 错误消息（经典C风格）
char* error_msg(int code) {
    static char msg[256];   // ⚠️ 非线程安全
    snprintf(msg, sizeof(msg), "Error: %d", code);
    return msg;
}

// 线程存储期 - 线程安全错误消息
_Thread_local static char tls_msg[256];
char* error_msg_threadsafe(int code) {
    snprintf(tls_msg, sizeof(tls_msg), "Error: %d", code);
    return tls_msg;
}

// 分配存储期 - 动态错误对象
Error* create_error(int code, const char* msg) {
    Error* e = malloc(sizeof(Error));
    e->code = code;
    e->message = strdup(msg);
    return e;
}
