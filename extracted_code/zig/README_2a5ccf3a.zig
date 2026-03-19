//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\README.md
// Line: 357
// Language: zig
// Block ID: 2a5ccf3a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// 检查生成的汇编代码
export fn inspect_call_convention(a: i64, b: f64, c: i32) i64 {
    // 编译并检查:
    // zig build-obj -O ReleaseFast -target x86_64-linux-gnu
    //
    // 期望的寄存器分配 (System-V):
    // a -> %rdi
    // b -> %xmm0
    // c -> %esi
    return a + @as(i64, @intFromFloat(b)) + c;
}
