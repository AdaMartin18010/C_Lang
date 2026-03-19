//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
// Line: 608
// Language: zig
// Block ID: cbfd15c6
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 显式内存序完整示例
const std = @import("std");
const Atomic = std.atomic.Value;

const SharedObject = struct {
    ref_count: Atomic(i32),
    data: ?*anyopaque,
};

fn createObject() *SharedObject {
    const obj = std.heap.page_allocator.create(SharedObject) catch unreachable;
    obj.ref_count = Atomic(i32).init(1);  // 初始化
    obj.data = null;
    return obj;
}

fn retain(obj: *SharedObject) void {
    // Relaxed: 仅计数，不需要同步
    _ = obj.ref_count.fetchAdd(1, .Monotonic);
}

fn releaseObject(obj: *SharedObject) void {
    // Release: 确保之前的写入可见
    if (obj.ref_count.fetchSub(1, .Release) == 1) {
        // Acquire: 看到 count==0 后，确保看到所有 retain 的操作
        std.atomic.fence(.Acquire);
        if (obj.data) |data| {
            std.heap.page_allocator.destroy(@as(*SharedObject, @ptrCast(data)));
        }
        std.heap.page_allocator.destroy(obj);
    }
}

// Zig 比较交换
fn compareAndSet(ptr: *Atomic(i32), expected: i32, desired: i32) bool {
    // tryCmpxchg: 返回 ?i32（旧值），成功时返回 null
    const result = ptr.tryCmpxchg(
        expected,           // 期望值
        desired,            // 新值
        .AcqRel,            // 成功内存序
        .Monotonic          // 失败内存序
    );
    return result == null;  // null 表示成功
}

// Zig cmpxchg 返回结构体版本
fn compareAndSetStrong(ptr: *Atomic(i32), expected: i32, desired: i32) bool {
    const result = ptr.cmpxchg(
        expected,
        desired,
        .AcqRel,
        .Monotonic
    );
    return result == null;
}
