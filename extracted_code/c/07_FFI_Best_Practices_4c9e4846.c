/*
 * Auto-generated from: 15_Memory_Safe_Languages\01_Rust_vs_C\07_FFI_Best_Practices.md
 * Line: 232
 * Language: c
 * Block ID: 4c9e4846
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// mylib.h - C库头文件
#ifndef MYLIB_H
#define MYLIB_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 错误码枚举
typedef enum {
    MYLIB_OK = 0,
    MYLIB_ERROR_INVALID_PARAM = -1,
    MYLIB_ERROR_OUT_OF_MEMORY = -2,
    MYLIB_ERROR_IO = -3,
} mylib_status;

// 不透明句柄
typedef struct mylib_context* mylib_handle_t;

// 配置结构体
typedef struct {
    uint32_t version;
    size_t buffer_size;
    const char* config_path;
} mylib_config;

// API函数
mylib_status mylib_init(const mylib_config* config, mylib_handle_t* handle);
mylib_status mylib_process(mylib_handle_t handle, const uint8_t* input, size_t len);
void mylib_cleanup(mylib_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif // MYLIB_H
