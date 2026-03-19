/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\07_Zig_C_Interoperability.md
 * Line: 471
 * Language: c
 * Block ID: 87ac911e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// zigmath.h
#ifndef ZIGMATH_H
#define ZIGMATH_H

#ifdef __cplusplus
extern "C" {
#endif

// 基本函数
int zig_add(int a, int b);
int zig_divide(int a, int b, int *result);

// 结构体
typedef struct {
    float x;
    float y;
    float z;
} Vector;

// 向量操作
Vector create_vector(float x, float y, float z);
float vector_length(Vector v);

#ifdef __cplusplus
}
#endif

#endif
