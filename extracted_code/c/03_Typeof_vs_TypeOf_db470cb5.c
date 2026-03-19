/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 2119
 * Language: c
 * Block ID: db470cb5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 泛型数学函数

#ifndef GENERIC_MATH_H
#define GENERIC_MATH_H

#include <math.h>
#include <stdbool.h>

// 泛型绝对值
#define ABS(x) _Generic((x), \
    int: abs, \
    long: labs, \
    long long: llabs, \
    float: fabsf, \
    double: fabs, \
    long double: fabsl \
)(x)

// 使用 typeof 的平方宏
#define SQUARE(x) ({ \
    typeof(x) _x = (x); \
    _x * _x; \
})

// 泛型限制函数
#define CLAMP(x, min, max) ({ \
    typeof(x) _x = (x); \
    typeof(min) _min = (min); \
    typeof(max) _max = (max); \
    _x < _min ? _min : (_x > _max ? _max : _x); \
})

// 泛型线性插值
#define LERP(a, b, t) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    typeof(t) _t = (t); \
    _a + (_b - _a) * _t; \
})

#endif
