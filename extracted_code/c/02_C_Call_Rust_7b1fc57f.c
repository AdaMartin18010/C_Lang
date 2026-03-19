/*
 * Auto-generated from: 03_System_Technology_Domains\16_Rust_Interoperability\02_C_Call_Rust.md
 * Line: 405
 * Language: c
 * Block ID: 7b1fc57f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* rust_math.h - 由cbindgen自动生成 */
#ifndef RUST_MATH_H
#define RUST_MATH_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 初始化 */
void rust_math_init(void);

/* 基本数学 */
int32_t rust_add(int32_t a, int32_t b);
int32_t rust_factorial(int32_t n);
const char* rust_version(void);

/* 几何类型 */
typedef struct {
    double x;
    double y;
} RustPoint;

typedef struct {
    RustPoint center;
    double radius;
} RustCircle;

RustPoint rust_point_new(double x, double y);
double rust_point_distance(const RustPoint *a, const RustPoint *b);
double rust_circle_area(const RustCircle *c);

/* 不透明类型 */
typedef struct RustStringBuilder RustStringBuilder;
typedef struct RustIntVector RustIntVector;

/* StringBuilder */
RustStringBuilder* rust_string_builder_new(void);
void rust_string_builder_append(RustStringBuilder *builder, const char *s);
char* rust_string_builder_get(const RustStringBuilder *builder);
void rust_string_builder_free(RustStringBuilder *builder);
void rust_string_free(char *s);

/* IntVector */
RustIntVector* rust_int_vector_new(void);
void rust_int_vector_push(RustIntVector *vec, int32_t value);
const int32_t* rust_int_vector_as_slice(const RustIntVector *vec, size_t *len);
size_t rust_int_vector_copy(const RustIntVector *vec, int32_t *out, size_t max);
void rust_int_vector_free(RustIntVector *vec);

#ifdef __cplusplus
}
#endif

#endif /* RUST_MATH_H */
