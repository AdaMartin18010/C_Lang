//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
// Line: 762
// Language: zig
// Block ID: 6fcd1b75
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 的 TBAA 策略
// Zig 编译器基于 LLVM，继承其 TBAA 系统
// 但提供语言级别的控制

// 默认 TBAA 行为
fn defaultTBAA(ip: *i32, fp: *f32) void {
    ip.* = 1;
    fp.* = 2.0;
    const x = ip.*;
    // 类似 C，Zig 编译器可能假设 ip 和 fp 不别名
    _ = x;
}

// 使用 volatile 禁用优化
fn volatileAccess(ip: *i32, fp: *f32) void {
    const vip: *volatile i32 = ip;
    const vfp: *volatile f32 = fp;

    vip.* = 1;
    vfp.* = 2.0;
    const x = vip.*;
    _ = x;
}

// 使用 black_box 防止优化
fn blackBoxAccess(ip: *i32, fp: *f32) void {
    ip.* = 1;
    std.mem.doNotOptimizeAway(fp);  // 告诉编译器 fp 有副作用
    fp.* = 2.0;
    const x = ip.*;
    _ = x;
}

// Zig 的 @setRuntimeSafety 控制运行时检查
fn runtimeSafetyControl() void {
    @setRuntimeSafety(false);  // 禁用运行时安全检查
    // ...
}
