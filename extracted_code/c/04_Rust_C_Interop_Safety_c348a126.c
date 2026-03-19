/*
 * Auto-generated from: 08_Zig_C_Connection\04_Rust_C_Interop_Safety.md
 * Line: 2208
 * Language: c
 * Block ID: c348a126
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* union_example.h */
#ifndef UNION_EXAMPLE_H
#define UNION_EXAMPLE_H

#include <stdint.h>

typedef enum {
    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOLEAN
} ValueType;

typedef struct {
    ValueType type;
    union {
        int64_t int_value;
        double float_value;
        char* string_value;
        int bool_value;
    } data;
} Value;

/* 构造函数 */
Value* value_create_integer(int64_t v);
Value* value_create_float(double v);
Value* value_create_string(const char* v);
Value* value_create_boolean(int v);

/* 析构函数 */
void value_destroy(Value* v);

/* 访问函数 */
int64_t value_get_integer(const Value* v);
double value_get_float(const Value* v);
const char* value_get_string(const Value* v);
int value_get_boolean(const Value* v);

/* 工具函数 */
void value_print(const Value* v);

#endif
