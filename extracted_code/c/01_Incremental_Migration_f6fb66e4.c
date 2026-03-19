/*
 * Auto-generated from: 15_Memory_Safe_Languages\02_Migration_Strategies\01_Incremental_Migration.md
 * Line: 220
 * Language: c
 * Block ID: f6fb66e4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ffi_interface.h */
#ifndef FFI_INTERFACE_H
#define FFI_INTERFACE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 不透明句柄 - 隐藏实现细节 */
typedef struct RustBuffer* rust_buffer_t;
typedef struct RustParser* rust_parser_t;

/* 错误码定义 */
typedef enum {
    RUST_OK = 0,
    RUST_ERR_NULL_PTR = -1,
    RUST_ERR_INVALID_INPUT = -2,
    RUST_ERR_OOM = -3,
    RUST_ERR_INTERNAL = -4
} rust_error_t;

/* 生命周期管理 */
rust_buffer_t rust_buffer_create(size_t capacity);
void rust_buffer_destroy(rust_buffer_t buffer);
rust_error_t rust_buffer_append(rust_buffer_t buffer,
                                 const uint8_t* data,
                                 size_t len);
size_t rust_buffer_len(rust_buffer_t buffer);
const uint8_t* rust_buffer_data(rust_buffer_t buffer);

/* 解析器API */
rust_parser_t rust_parser_new(void);
void rust_parser_free(rust_parser_t parser);
rust_error_t rust_parser_feed(rust_parser_t parser,
                               const char* input,
                               size_t len);

#ifdef __cplusplus
}
#endif

#endif /* FFI_INTERFACE_H */
