/*
 * Auto-generated from: 07_Modern_Toolchain\12_Modern_C_Standards\01_C11_to_C23.md
 * Line: 37
 * Language: c
 * Block ID: fda51d9f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* _Generic: 类型安全的泛型 */
#define max(a, b) _Generic((a) + (b), \
    int: max_int, \
    double: max_double, \
    default: max_generic \
)(a, b)

int max_int(int a, int b) { return a > b ? a : b; }
double max_double(double a, double b) { return a > b ? a : b; }

/* 使用 */
int mi = max(1, 2);           /* 调用max_int */
double md = max(1.0, 2.0);    /* 调用max_double */

/* _Static_assert: 编译时断言 */
_Static_assert(sizeof(int) == 4, "int must be 32-bit");

/* 匿名结构体和联合体 */
struct {
    union {
        struct { int x, y; };  /* 匿名 */
        int coords[2];
    };
} point;

point.x = 10;  /* 直接访问 */
point.coords[1] = 20;

/* 原子操作 */
#include <stdatomic.h>
_Atomic(int) counter = 0;
atomic_fetch_add(&counter, 1);

/* 线程支持 */
#include <threads.h>
thrd_t thread;
thrd_create(&thread, func, arg);

/* 复数支持 */
#include <complex.h>
double complex z = 1.0 + 2.0*I;

/* alignas/alignof */
alignas(64) char cache_line[64];
_Static_assert(alignof(max_align_t) >= 8, "");
