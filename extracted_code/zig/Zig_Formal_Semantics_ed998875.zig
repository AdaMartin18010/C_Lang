//
// Auto-generated from: Zig\Zig_Formal_Semantics.md
// Line: 933
// Language: zig
// Block ID: ed998875
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 类型安全的 Zig 程序示例

// 安全: 编译时类型检查防止类型错误
fn typeSafeExample() void {
    const x: i32 = 10;
    // const y: f32 = x;  // 编译错误: 类型不匹配
    const y: f32 = @floatFromInt(x);  // 显式转换
    _ = y;

    // 安全: 可选类型强制检查
    const opt: ?i32 = 42;
    // const v: i32 = opt;  // 编译错误
    const v: i32 = opt.?;  // 显式解包
    _ = v;

    // 安全: 错误联合类型强制处理
    const result: error{Overflow}!i32 = 42;
    // const w: i32 = result;  // 编译错误
    const w: i32 = result catch 0;  // 显式处理
    _ = w;
}

// 内存安全: 编译时检查防止悬垂指针
fn memorySafety(allocator: std.mem.Allocator) !void {
    // 安全: 栈变量生命周期自动管理
    {
        const local: i32 = 10;
        _ = local;
    }  // local 在这里无效

    // 安全: 堆分配必须显式管理
    const heap_ptr = try allocator.create(i32);
    defer allocator.destroy(heap_ptr);  // 保证释放

    heap_ptr.* = 20;

    // 安全: 数组边界检查 (调试模式)
    var arr = [3]i32{ 1, 2, 3 };
    // arr[5] = 0;  // 运行时错误 (调试模式)

    // 安全: 切片长度检查
    const slice = arr[0..2];
    // _ = slice[5];  // 运行时错误
    _ = slice;
}

// 并发安全: 原子操作和数据竞争检测
fn concurrencySafety() void {
    var shared = std.atomic.Value(i32).init(0);

    // 安全: 原子操作保证可见性
    shared.store(1, .release);

    // 安全: 原子读取
    const val = shared.load(.acquire);
    _ = val;

    // 注意: Zig 目前不提供编译时数据竞争检测
    // 需要使用 ThreadSanitizer 等工具
}

const std = @import("std");
