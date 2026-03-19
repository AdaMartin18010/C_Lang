/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 3943
 * Language: c
 * Block ID: 2436d4d4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file assert_utils.h
 * @brief 断言和错误处理
 */

#ifndef ASSERT_UTILS_H
#define ASSERT_UTILS_H

#include "pico/stdlib.h"

// 自定义断言
#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            assertion_failed(#cond, __FILE__, __LINE__, __func__); \
        } \
    } while(0)

#define ASSERT_MSG(cond, msg) \
    do { \
        if (!(cond)) { \
            assertion_failed_msg(#cond, msg, __FILE__, __LINE__, __func__); \
        } \
    } while(0)

// 断言失败处理
static inline void assertion_failed(const char* condition,
                                     const char* file, int line,
                                     const char* func) {
    printf("\n\n!!! ASSERTION FAILED !!!\n");
    printf("Condition: %s\n", condition);
    printf("File: %s\n", file);
    printf("Line: %d\n", line);
    printf("Function: %s\n", func);
    printf("\n");

    // 闪烁 LED 指示错误
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    while (1) {
        gpio_put(25, 1);
        sleep_ms(100);
        gpio_put(25, 0);
        sleep_ms(100);
    }
}

static inline void assertion_failed_msg(const char* condition,
                                         const char* msg,
                                         const char* file, int line,
                                         const char* func) {
    printf("\n\n!!! ASSERTION FAILED !!!\n");
    printf("Condition: %s\n", condition);
    printf("Message: %s\n", msg);
    printf("File: %s\n", file);
    printf("Line: %d\n", line);
    printf("Function: %s\n", func);
    printf("\n");

    while (1) {
        tight_loop_contents();
    }
}

// 错误代码
typedef enum {
    ERR_OK = 0,
    ERR_INVALID_PARAM,
    ERR_OUT_OF_MEMORY,
    ERR_TIMEOUT,
    ERR_NOT_FOUND,
    ERR_IO,
    ERR_BUSY,
    ERR_UNKNOWN
} error_code_t;

// 检查返回码
#define CHECK_ERROR(code) \
    do { \
        error_code_t _err = (code); \
        if (_err != ERR_OK) { \
            printf("Error at %s:%d: %d\n", __FILE__, __LINE__, _err); \
            return _err; \
        } \
    } while(0)

#endif // ASSERT_UTILS_H
