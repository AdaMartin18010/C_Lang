//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
// Line: 717
// Language: zig
// Block ID: 87098d07
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码: 向量类型

// Zig 提供 @Vector 类型
const Vec4f = @Vector(4, f32);    // 128-bit SSE 向量
const Vec8f = @Vector(8, f32);    // 256-bit AVX 向量
const Vec16f = @Vector(16, f32);  // 512-bit AVX-512 向量

// 使用 C ABI 传递向量
extern fn sseVec(v: @Vector(4, f32)) void;
extern fn avxVec(v: @Vector(8, f32)) void;

// 注意: Zig 的 @Vector 需要目标支持相应指令集
// 编译时检查: @hasFeature("sse2"), @hasFeature("avx")
