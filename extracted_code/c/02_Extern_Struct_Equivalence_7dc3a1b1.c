/*
 * Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
 * Line: 2138
 * Language: c
 * Block ID: 7dc3a1b1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码 - library.h (FFI 接口)
#ifndef LIBRARY_H
#define LIBRARY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 不透明句柄
typedef struct Context* ContextPtr;

// 选项结构体
struct Options {
    uint32_t version;           // 结构体版本
    uint32_t flags;             // 行为标志
    uint16_t timeout_ms;        // 超时毫秒
    uint16_t retry_count;       // 重试次数
    uint32_t buffer_size;       // 缓冲区大小
    const char* config_path;    // 配置文件路径
};

// 结果结构体
struct Result {
    int32_t code;               // 返回码
    uint32_t data_len;          // 数据长度
    const uint8_t* data;        // 数据指针 (由库管理)
};

// 回调函数类型
typedef void (*DataCallback)(const struct Result* result, void* user_data);
typedef void (*LogCallback)(int level, const char* message);

// API 函数
ContextPtr context_create(const struct Options* options);
void context_destroy(ContextPtr ctx);
int context_execute(ContextPtr ctx, const uint8_t* input, uint32_t len, DataCallback cb, void* user_data);
void set_log_callback(LogCallback cb);

// 版本信息
const char* get_version(void);
uint32_t get_api_version(void);

#ifdef __cplusplus
}
#endif

#endif
