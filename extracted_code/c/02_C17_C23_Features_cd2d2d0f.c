/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\02_C17_C23_Features.md
 * Line: 88
 * Language: c
 * Block ID: cd2d2d0f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 类型推导
int x = 42;
typeof(x) y = 100;  // y也是int

typeof(int[10]) arr;  // arr是int[10]类型

// 与auto配合
auto p = &x;        // p是int*
typeof(*p) z = 0;   // z是int

// 用于宏，避免多次求值
#define MAX(a, b) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    _a > _b ? _a : _b; \
})

// typeof_unqual - 移除类型限定
const int ci = 10;
typeof_unqual(ci) uci = 20;  // uci是int，不是const int
