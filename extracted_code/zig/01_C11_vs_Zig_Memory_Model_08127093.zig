//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
// Line: 189
// Language: zig
// Block ID: 08127093
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig Relaxed 内存序示例
const std = @import("std");
const Atomic = std.atomic.Value;

var counter = Atomic(u64).init(0);

fn incrementRelaxed() void {
    // Relaxed 递增 - 无同步保证
    _ = counter.fetchAdd(1, .Monotonic);
}

fn getRelaxed() u64 {
    // Relaxed 读取
    return counter.load(.Monotonic);
}
