//
// Auto-generated from: Zig\Zig_Formal_Semantics.md
// Line: 327
// Language: zig
// Block ID: cc9a41fa
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生命周期分析 (类似 Rust 的所有权，但显式控制)

// 问题: 悬垂指针
// fn danglingPointer() *i32 {
//     var x: i32 = 10;
//     return &x;  // 错误: 返回栈变量的地址
// }

// 正确做法: 使用堆分配或参数传递
fn validPointer(allocator: std.mem.Allocator) !*i32 {
    const ptr = try allocator.create(i32);
    ptr.* = 10;
    return ptr;  // OK: 堆内存生命周期独立于函数
}

// 借用语义
fn borrowExample() void {
    var data: [100]u8 = undefined;

    // 可变借用
    const slice_mut: []u8 = &data;
    slice_mut[0] = 1;

    // 不可变借用
    const slice_const: []const u8 = &data;
    _ = slice_const[0];

    // Zig 允许同时多个不可变借用，或单个可变借用
    // (编译时检查，非运行时)
}

// defer/errdefer - 确定性资源释放
fn resourceManagement(allocator: std.mem.Allocator) !void {
    const resource = try acquireResource();
    defer releaseResource(resource);  // 函数退出时执行

    const buffer = try allocator.alloc(u8, 100);
    errdefer allocator.free(buffer);  // 仅错误时执行

    // 可能失败的操作
    try riskyOperation();

    // 成功时手动释放
    allocator.free(buffer);
}

fn acquireResource() !*Resource { return undefined; }
fn releaseResource(r: *Resource) void { _ = r; }
fn riskyOperation() !void { return error.Failed; }
const Resource = opaque {};
