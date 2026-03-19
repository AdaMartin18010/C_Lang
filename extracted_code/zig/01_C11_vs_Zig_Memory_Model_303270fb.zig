//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
// Line: 1129
// Language: zig
// Block ID: 303270fb
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 基本原子操作完整示例
const std = @import("std");
const Atomic = std.atomic.Value;

// 声明原子变量
var counter = Atomic(i32).init(0);
var big_counter = Atomic(u64).init(0);
var flag = Atomic(bool).init(false);
var ptr = Atomic(?*anyopaque).init(null);

fn basicOperations() void {
    // 加载和存储
    const val = counter.load(.SeqCst);
    counter.store(100, .SeqCst);

    // 显式内存序
    counter.store(200, .Release);
    const val2 = counter.load(.Acquire);
    _ = val2;

    // 获取并修改
    const old1 = counter.fetchAdd(1, .SeqCst);   // ++counter
    const old2 = counter.fetchSub(1, .SeqCst);   // --counter
    const old3 = counter.fetchAnd(0xFF, .SeqCst); // counter &= 0xFF
    const old4 = counter.fetchOr(0x10, .SeqCst);  // counter |= 0x10
    const old5 = counter.fetchXor(0x01, .SeqCst); // counter ^= 0x01
    _ = old1; _ = old2; _ = old3; _ = old4; _ = old5;

    // 交换
    const prev = counter.swap(999, .SeqCst);
    _ = prev;

    // 比较交换 - Zig 两种 API
    // 1. cmpxchg: 返回包含旧值和成功标志的结构体
    const result1 = counter.cmpxchg(999, 1000, .SeqCst, .SeqCst);
    // result1.old_value, result1.success

    // 2. tryCmpxchg: 返回 ?T，成功时返回 null，失败返回当前值
    const result2 = counter.tryCmpxchg(1000, 2000, .SeqCst, .SeqCst);
    if (result2) |old_value| {
        // 失败，old_value 是当前值
        _ = old_value;
    } else {
        // 成功
    }
}

// 不同内存序的 CAS
fn casWithOrder() void {
    const exp: i32 = 0;
    const result = counter.tryCmpxchg(
        exp,
        1,
        .AcqRel,    // 成功时内存序
        .Monotonic  // 失败时内存序
    );
    _ = result;
}
