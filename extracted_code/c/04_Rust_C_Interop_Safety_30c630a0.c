/*
 * Auto-generated from: 08_Zig_C_Connection\04_Rust_C_Interop_Safety.md
 * Line: 545
 * Language: c
 * Block ID: 30c630a0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 对应的C结构体定义 */
#ifndef RUST_STRUCTS_H
#define RUST_STRUCTS_H

/* 点结构体 */
typedef struct {
    double x;
    double y;
} Point;

/* 人员结构体 */
typedef struct {
    const char *name;
    int age;
} Person;

/* 缓冲区结构体 */
typedef struct {
    unsigned char data[1024];
    size_t len;
} Buffer;

/* 枚举 */
typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR = 1,
    STATUS_WARNING = 2
} Status;

/* 结果结构体 */
typedef struct {
    Status status;
    double value;
} Result;

#endif
