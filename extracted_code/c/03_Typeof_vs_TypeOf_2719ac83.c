/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 1045
 * Language: c
 * Block ID: 2719ac83
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 使用 typeof 实现类型安全宏

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// ========== 基础泛型宏 ==========

// 类型安全最大值宏
#define MAX(a, b) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    _a > _b ? _a : _b; \
})

// 类型安全最小值宏
#define MIN(a, b) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    _a < _b ? _a : _b; \
})

// 类型安全交换宏
#define SWAP(a, b) do { \
    typeof_unqual(a) _tmp = (a); \
    (a) = (b); \
    (b) = _tmp; \
} while(0)

// ========== 容器类宏 ==========

// 泛型数组大小
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(typeof((arr)[0])))

// 泛型清零
#define ZERO_INIT(obj) memset(&(obj), 0, sizeof(typeof(obj)))

// ========== 类型特征萃取 ==========

// 获取元素类型
#define ELEMENT_TYPE(arr) typeof((arr)[0])

// 获取无限定符元素类型
#define UNQUAL_ELEMENT_TYPE(arr) typeof_unqual((arr)[0])

// ========== 调试宏 ==========

#define PRINT_TYPE(x) printf("%s is of type %s\n", #x, _Generic((x), \
    int: "int", \
    long: "long", \
    float: "float", \
    double: "double", \
    default: "unknown" \
))

// ========== 使用示例 ==========

void demo_typeof_macros(void) {
    // MAX/MIN 使用
    int i1 = 10, i2 = 20;
    int m = MAX(i1, i2);
    printf("MAX(%d, %d) = %d\n", i1, i2, m);

    double d1 = 3.14, d2 = 2.71;
    double dm = MAX(d1, d2);
    printf("MAX(%f, %f) = %f\n", d1, d2, dm);

    // SWAP 使用
    int a = 1, b = 2;
    SWAP(a, b);
    printf("After SWAP: a=%d, b=%d\n", a, b);

    // ARRAY_SIZE 使用
    int arr[100];
    printf("Array size: %zu\n", ARRAY_SIZE(arr));

    // ZERO_INIT 使用
    struct { int x; int y; } point = {1, 2};
    ZERO_INIT(point);
    printf("After zero: x=%d, y=%d\n", point.x, point.y);
}
