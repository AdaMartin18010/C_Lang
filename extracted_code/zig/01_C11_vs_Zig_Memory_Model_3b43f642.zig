//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
// Line: 273
// Language: zig
// Block ID: 3b43f642
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig Acquire-Release 语义示例
const std = @import("std");
const Atomic = std.atomic.Value;

var flag = Atomic(bool).init(false);
var data: i32 = 0;

// 生产者线程
fn producer() void {
    data = 42;
    // Release: 确保此操作之前的写入对其他线程可见
    flag.store(true, .Release);
}

// 消费者线程
fn consumer() void {
    // Acquire: 确保看到 flag=true 后，能看到 producer 的写入
    while (flag.load(.Acquire) != true) {
        std.atomic.spinLoopHint();
    }
    flag.store(false, .Release);

    // 现在可以安全读取 data
    const result = data;  // 保证看到 42
    _ = result;
}
