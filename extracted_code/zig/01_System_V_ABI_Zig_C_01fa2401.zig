//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
// Line: 888
// Language: zig
// Block ID: 01fa2401
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码: 寄存器传递的结构体

// 8字节结构体
const Small = extern struct { x: i32 };
extern fn smallStruct(s: Small) void;
// %edi

// 16字节结构体
const Medium = extern struct { x: i64, y: i64 };
extern fn mediumStruct(s: Medium) void;
// %rdi, %rsi

// 含浮点的结构体
const Mixed = extern struct {
    i: i32,
    f: f32,
};
extern fn mixedStruct(s: Mixed) void;
// 与 C 相同: %rdi, %xmm0

// 验证布局等价性
comptime {
    // 确保 Zig 布局与 C 相同
    assert(@sizeOf(Mixed) == 8);
    assert(@offsetOf(Mixed, "i") == 0);
    assert(@offsetOf(Mixed, "f") == 4);
}
