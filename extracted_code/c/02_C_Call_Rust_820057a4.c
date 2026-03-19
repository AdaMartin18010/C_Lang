/*
 * Auto-generated from: 03_System_Technology_Domains\16_Rust_Interoperability\02_C_Call_Rust.md
 * Line: 471
 * Language: c
 * Block ID: 820057a4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* main.c - 使用Rust库的C程序 */
#include <stdio.h>
#include "rust_math.h"

int main() {
    /* 初始化 */
    rust_math_init();

    /* 基本数学 */
    printf("2 + 3 = %d\n", rust_add(2, 3));
    printf("5! = %d\n", rust_factorial(5));
    printf("Version: %s\n", rust_version());

    /* 几何计算 */
    RustPoint a = rust_point_new(0.0, 0.0);
    RustPoint b = rust_point_new(3.0, 4.0);
    printf("Distance: %.2f\n", rust_point_distance(&a, &b));

    /* 使用StringBuilder */
    RustStringBuilder *sb = rust_string_builder_new();
    rust_string_builder_append(sb, "Hello");
    rust_string_builder_append(sb, " ");
    rust_string_builder_append(sb, "World!");

    char *result = rust_string_builder_get(sb);
    printf("Result: %s\n", result);
    rust_string_free(result);  /* 重要：释放Rust分配的内存 */
    rust_string_builder_free(sb);

    /* 使用IntVector */
    RustIntVector *vec = rust_int_vector_new();
    for (int i = 0; i < 10; i++) {
        rust_int_vector_push(vec, i * i);
    }

    size_t len;
    const int32_t *data = rust_int_vector_as_slice(vec, &len);
    printf("Vector (%zu elements): ", len);
    for (size_t i = 0; i < len; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");

    rust_int_vector_free(vec);

    return 0;
}
