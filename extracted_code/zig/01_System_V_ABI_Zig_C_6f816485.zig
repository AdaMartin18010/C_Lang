//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
// Line: 397
// Language: zig
// Block ID: 6f816485
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码: 对应的函数声明

// 单个整数 - INTEGER 类
extern fn f1(x: i32) void;         // %rdi

// 多个整数 - 依次使用寄存器
extern fn f2(a: i32, b: i32, c: i32,    // %rdi, %rsi, %rdx
             d: i32, e: i32, f: i32)    // %rcx, %r8, %r9
             void;

// 第7个整数 - 使用栈
extern fn f3(a: i32, b: i32, c: i32,    // %rdi, %rsi, %rdx
             d: i32, e: i32, f: i32,    // %rcx, %r8, %r9
             g: i32)                    // 8(%rsp)
             void;

// 混合类型
extern fn f4(x: i32, y: f64,            // %rdi, %xmm0
             z: i64, w: f32)            // %rsi, %xmm1
             void;

// 结构体 - 使用 extern struct 保证布局兼容
const Point = extern struct { x: i64, y: i64 };
extern fn f5(p: Point) void;            // %rdi, %rsi

// 结构体 (含浮点)
const Vec2 = extern struct { x: f64, y: f64 };
extern fn f6(v: Vec2) void;             // %xmm0, %xmm1
