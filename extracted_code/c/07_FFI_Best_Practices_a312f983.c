/*
 * Auto-generated from: 15_Memory_Safe_Languages\01_Rust_vs_C\07_FFI_Best_Practices.md
 * Line: 574
 * Language: c
 * Block ID: a312f983
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Auto-generated C header */
#ifndef MY_RUST_LIB_H
#define MY_RUST_LIB_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* Warning: this file is auto-generated */

/**
 * 处理器配置
 */
typedef struct {
    uint32_t threads;
    size_t buffer_size;
    bool enable_compression;
} ProcessorConfig;

/**
 * 错误码
 */
typedef enum {
    Success = 0,
    InvalidArgument = 1,
    OutOfMemory = 2,
    ProcessingError = 3,
} ErrorCode;

/**
 * 不透明上下文类型
 */
typedef struct ProcessorContext ProcessorContext;

/**
 * 创建处理器
 *
 * # Safety
 * config必须是有效的非空指针
 * out_context必须是有效的可写指针
 */
ErrorCode processor_create(const ProcessorConfig *config,
                           ProcessorContext **out_context);

/**
 * 处理数据
 *
 * # Safety
 * context必须是有效的处理器上下文
 * data可以是空（当len为0时）
 */
ErrorCode processor_process(ProcessorContext *context,
                            const uint8_t *data,
                            size_t len);

/**
 * 销毁处理器
 *
 * # Safety
 * context必须是有效的由processor_create返回的指针，或NULL
 */
void processor_destroy(ProcessorContext *context);

#endif /* MY_RUST_LIB_H */
