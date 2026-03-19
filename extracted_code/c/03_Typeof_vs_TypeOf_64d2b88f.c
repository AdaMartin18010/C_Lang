/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 137
 * Language: c
 * Block ID: 64d2b88f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 typeof 基本语法
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// ========== 基本用法 ==========

void basic_usage(void) {
    int x = 42;

    // 从表达式推导类型
    typeof(x) y = 100;           // y 是 int
    typeof(3.14) pi = 3.14159;   // pi 是 double
    typeof("hello") str;         // str 是 char[6] (包含 '\0')

    // 从类型名推导类型
    typeof(int) z = 200;         // z 是 int
    typeof(double) d = 2.718;    // d 是 double
}

// ========== 复杂表达式 ==========

void complex_expressions(void) {
    int arr[10];
    int* ptr = arr;

    // 数组推导
    typeof(arr) arr2;            // int[10] - 完整数组类型
    typeof(arr[0]) elem;         // int - 数组元素类型

    // 指针推导
    typeof(ptr) ptr2;            // int*
    typeof(*ptr) val;            // int - 解引用类型
    typeof(&val) ptr3;           // int* - 取地址类型

    // 函数调用表达式
    int func(double);
    typeof(func(1.0)) ret;       // int - 返回类型
    typeof(func) fn_ptr;         // 函数类型 (退化为函数指针)
}

// ========== 类型限定符保留 ==========

void qualifier_preservation(void) {
    const int cx = 42;
    volatile int vy = 100;
    const volatile int cvz = 0;

    // typeof 保留限定符
    typeof(cx) cx2 = 10;         // const int
    typeof(vy) vy2 = 20;         // volatile int
    typeof(cvz) cvz2 = 30;       // const volatile int

    // 注意: cx2 不能修改 (const 保留)
    // cx2 = 5;  // ERROR: assignment of read-only variable
}
