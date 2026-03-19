/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 2600
 * Language: c
 * Block ID: e8cd14a0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 运行时性能分析

#include <time.h>
#include <stdio.h>

// typeof 不影响运行时性能，它只是编译时类型推导
// 生成的代码与显式类型声明完全相同

void c23_runtime_performance(void) {
    // 这两者在运行时性能上完全相同:
    int x1 = 42;
    typeof(int) x2 = 42;
    typeof(x1) x3 = 42;

    // typeof 在宏中的应用也无运行时开销
    #define TYPEOF_MAX(a, b) ({ \
        typeof(a) _a = (a); \
        typeof(b) _b = (b); \
        _a > _b ? _a : _b; \
    })

    // 展开后与手写代码相同
    int m = TYPEOF_MAX(10, 20);
    // 等价于:
    // int m = ({ int _a = 10; int _b = 20; _a > _b ? _a : _b; });
}
