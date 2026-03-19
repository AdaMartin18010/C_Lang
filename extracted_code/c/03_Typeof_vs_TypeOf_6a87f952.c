/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 2221
 * Language: c
 * Block ID: 6a87f952
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 有限的类型特征萃取

#ifndef TYPE_TRAITS_H
#define TYPE_TRAITS_H

#include <stdbool.h>
#include <stdint.h>

// 基本类型大小
#define SIZEOF(T) sizeof(T)
#define ALIGNOF(T) alignof(T)

// 检查是否为整数类型 (有限)
#define IS_INTEGER_TYPE(T) _Generic((T){0}, \
    int: true, \
    long: true, \
    long long: true, \
    unsigned int: true, \
    unsigned long: true, \
    unsigned long long: true, \
    default: false \
)

// 获取基本类型的最大值 (有限)
#define MAX_VALUE(T) _Generic((T){0}, \
    int: INT_MAX, \
    long: LONG_MAX, \
    long long: LLONG_MAX, \
    unsigned int: UINT_MAX, \
    unsigned long: ULONG_MAX, \
    unsigned long long: ULLONG_MAX \
)

// 检查数组类型
#define IS_ARRAY(x) (sizeof(x) != sizeof(&(x)[0]))

// 获取数组元素类型
#define ARRAY_ELEMENT_TYPE(arr) typeof((arr)[0])

// 获取数组大小
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

#endif
