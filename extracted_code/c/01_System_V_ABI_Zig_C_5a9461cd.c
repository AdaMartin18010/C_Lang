/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
 * Line: 369
 * Language: c
 * Block ID: 5a9461cd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: 参数分类示例

// 单个整数 - INTEGER 类
void f1(int x);                    // %rdi

// 多个整数 - 依次使用寄存器
void f2(int a, int b, int c,       // %rdi, %rsi, %rdx
        int d, int e, int f);      // %rcx, %r8, %r9

// 第7个整数 - 使用栈
void f3(int a, int b, int c,       // %rdi, %rsi, %rdx
        int d, int e, int f,       // %rcx, %r8, %r9
        int g);                    // 8(%rsp)

// 混合类型
void f4(int x, double y,           // %rdi, %xmm0
        long z, float w);          // %rsi, %xmm1

// 结构体 (16字节, 两个整数) - 两个 INTEGER 类
struct Point { long x, y; };       // %rdi, %rsi
void f5(struct Point p);

// 结构体 (含浮点) - SSE 类
struct Vec2 { double x, y; };      // %xmm0, %xmm1 (或 ymm0)
void f6(struct Vec2 v);
