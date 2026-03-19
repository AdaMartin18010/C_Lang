/*
 * Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
 * Line: 1872
 * Language: c
 * Block ID: 676e72db
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码 - basic_struct.c
#ifndef BASIC_STRUCT_H
#define BASIC_STRUCT_H

#include <stdint.h>

// 基础点结构体
struct Point {
    int32_t x;
    int32_t y;
};

// 3D 点
struct Point3D {
    struct Point base;
    int32_t z;
};

// 矩形
struct Rect {
    struct Point top_left;
    struct Point bottom_right;
};

// 颜色 (RGBA)
struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

// 混合类型结构体
struct Mixed {
    uint8_t type;
    uint16_t flags;
    uint32_t data;
    double timestamp;
};

#endif
