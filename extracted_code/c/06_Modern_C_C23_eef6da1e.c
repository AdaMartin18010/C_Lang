/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\06_Modern_C_C23.md
 * Line: 82
 * Language: c
 * Block ID: eef6da1e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 获取变量类型
int x = 42;
typeof(x) y = 100;  // y也是int

// 用于泛型宏
#define max(a, b) ({           \
    typeof(a) _a = (a);        \
    typeof(b) _b = (b);        \
    _a > _b ? _a : _b;         \
})

// 使用
int m1 = max(10, 20);        // 返回int
double m2 = max(3.14, 2.71); // 返回double
