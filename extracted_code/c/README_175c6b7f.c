/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\README.md
 * Line: 86
 * Language: c
 * Block ID: 175c6b7f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// include/myproject/module.h
#ifndef MYPROJECT_MODULE_H
#define MYPROJECT_MODULE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 版本信息
#define MYPROJECT_VERSION_MAJOR 1
#define MYPROJECT_VERSION_MINOR 0
#define MYPROJECT_VERSION_PATCH 0

// API 可见性宏
#ifdef MYPROJECT_BUILD_SHARED
    #ifdef _WIN32
        #ifdef MYPROJECT_EXPORTS
            #define MYPROJECT_API __declspec(dllexport)
        #else
            #define MYPROJECT_API __declspec(dllimport)
        #endif
    #else
        #define MYPROJECT_API __attribute__((visibility("default")))
    #endif
#else
    #define MYPROJECT_API
#endif

// 前向声明
struct myproject_context;
typedef struct myproject_context myproject_context_t;

// 错误码定义
typedef enum {
    MYPROJECT_OK = 0,
    MYPROJECT_ERROR_INVALID_PARAM = -1,
    MYPROJECT_ERROR_OUT_OF_MEMORY = -2,
    MYPROJECT_ERROR_IO = -3,
    MYPROJECT_ERROR_NOT_IMPLEMENTED = -4
} myproject_error_t;

// 生命周期管理
MYPROJECT_API myproject_context_t* myproject_create(void);
MYPROJECT_API void myproject_destroy(myproject_context_t* ctx);

// 功能API
MYPROJECT_API myproject_error_t myproject_do_work(
    myproject_context_t* ctx,
    const char* input,
    char* output,
    size_t output_size
);

#ifdef __cplusplus
}
#endif

#endif // MYPROJECT_MODULE_H
