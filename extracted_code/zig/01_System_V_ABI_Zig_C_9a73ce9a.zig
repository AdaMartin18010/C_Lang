//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
// Line: 664
// Language: zig
// Block ID: 9a73ce9a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码: x87 类型传递

// long double 映射
extern fn x87Type(ld: c_longdouble) void;
// 注意: Zig 的 c_longdouble 根据目标平台映射
// x86-64 Linux: 80-bit extended precision

// Zig 复数类型
const ComplexF64 = extern struct {
    re: f64,
    im: f64,
};

extern fn complexDouble(cd: ComplexF64) void;
// cd.re: %xmm0
// cd.im: %xmm1

// 注意: Zig 标准库提供 std.math.complex.Complex
// 但用于 C 互操作时应使用 extern struct
