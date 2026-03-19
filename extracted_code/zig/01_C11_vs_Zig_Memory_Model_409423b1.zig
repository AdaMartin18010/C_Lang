//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
// Line: 369
// Language: zig
// Block ID: 409423b1
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig Sequential Consistency 示例
const std = @import("std");
const Atomic = std.atomic.Value;

var x = Atomic(i32).init(0);
var y = Atomic(i32).init(0);

fn thread1() void {
    // SeqCst: 所有线程以相同顺序看到此操作
    x.store(1, .SeqCst);
    const r1 = y.load(.SeqCst);
    _ = r1;
    // r1 == 0 表示 Thread2 的 x=1 还没看到
}

fn thread2() void {
    y.store(1, .SeqCst);
    const r2 = x.load(.SeqCst);
    _ = r2;
    // r2 == 0 表示 Thread1 的 y=1 还没看到
}

// 不可能同时 r1 == 0 && r2 == 0（使用 SeqCst）
