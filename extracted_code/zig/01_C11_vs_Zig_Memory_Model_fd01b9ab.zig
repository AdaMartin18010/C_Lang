//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
// Line: 815
// Language: zig
// Block ID: fd01b9ab
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 同步规则演示
const std = @import("std");
const Atomic = std.atomic.Value;

var flag = Atomic(i32).init(0);
var data1: i32 = 0;
var data2: i32 = 0;

// 规则 1: Release-Acquire 配对
fn thread1Rule1() void {
    data1 = 1;
    data2 = 2;
    flag.store(1, .Release);
}

fn thread2Rule1() void {
    while (flag.load(.Acquire) != 1) {
        std.atomic.spinLoopHint();
    }
    // 保证看到 data1=1, data2=2
    std.debug.assert(data1 == 1);
    std.debug.assert(data2 == 2);
}

// 规则 2: 传递性
var flag2 = Atomic(i32).init(0);

fn thread1Rule2() void {
    data1 = 1;
    flag.store(1, .Release);
}

fn thread2Rule2() void {
    while (flag.load(.Acquire) != 1) {
        std.atomic.spinLoopHint();
    }
    data2 = 2;
    flag2.store(1, .Release);
}

fn thread3Rule2() void {
    while (flag2.load(.Acquire) != 1) {
        std.atomic.spinLoopHint();
    }
    // 通过传递性，保证看到 data1=1, data2=2
    std.debug.assert(data1 == 1);
    std.debug.assert(data2 == 2);
}

// 规则 3: SeqCst 全序
var x = Atomic(i32).init(0);
var y = Atomic(i32).init(0);

fn thread1Rule3() void {
    x.store(1, .SeqCst);
    const r1 = y.load(.SeqCst);
    _ = r1;
}

fn thread2Rule3() void {
    y.store(1, .SeqCst);
    const r2 = x.load(.SeqCst);
    _ = r2;
}
// 不可能 r1=0 && r2=0（使用 SeqCst）
