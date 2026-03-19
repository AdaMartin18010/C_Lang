/*
 * Auto-generated from: 03_System_Technology_Domains\16_Rust_Interoperability\README.md
 * Line: 547
 * Language: c
 * Block ID: d04a49b6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* rust_types.h - C 端使用的类型定义 */

#ifndef RUST_TYPES_H
#define RUST_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// 对应 Rust 的 Option<*mut T>
typedef struct {
    void *ptr;
    bool is_some;
} RustOptionPtr;

// 对应 Rust 的 Result<T, E>（简化）
typedef struct {
    int32_t value;
    uint32_t error_code;
    bool is_ok;
} RustResultI32;

// 字符串视图（零拷贝）
typedef struct {
    const char *ptr;
    size_t len;
} RustStr;

// 切片视图
typedef struct {
    const uint8_t *ptr;
    size_t len;
} RustSliceU8;

#endif
