/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 1999
 * Language: c
 * Block ID: d61a6756
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 类型安全宏实现

#ifndef TYPE_SAFE_MACROS_H
#define TYPE_SAFE_MACROS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// 类型安全最大值
#define MAX(a, b) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    _a > _b ? _a : _b; \
})

// 类型安全最小值
#define MIN(a, b) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    _a < _b ? _a : _b; \
})

// 类型安全交换 (使用 typeof_unqual 避免 const 问题)
#define SWAP(a, b) do { \
    typeof_unqual(a) _tmp = (a); \
    (a) = (b); \
    (b) = _tmp; \
} while(0)

// 类型安全数组长度
#define ARRAY_SIZE(arr) \
    (sizeof(arr) / sizeof(typeof((arr)[0])))

// 类型安全偏移量计算
#define OFFSET_OF(type, member) \
    ((size_t)&((type*)0)->member)

// 容器遍历
#define FOR_EACH(type, item, container, size) \
    for (type* item = (container); item < (container) + (size); item++)

#endif
