//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 1288
// Language: zig
// Block ID: 08420d0f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: comptime 有独立的内存空间

// comptime 可以分配内存
const LARGE_TABLE = comptime blk: {
    var table: [10000]i32 = undefined;
    for (0..10000) |i| {
        table[i] = @intCast(i * i);
    }
    break :blk table;
};

// comptime 使用 arena 分配器
const DYNAMIC_TABLE = comptime blk: {
    var list = std.ArrayList(i32).init(std.heap.page_allocator);
    defer list.deinit();

    var i: i32 = 0;
    while (i < 1000) : (i += 1) {
        list.append(i * i) catch unreachable;
    }

    // 将结果复制到常量
    var result: [1000]i32 = undefined;
    for (list.items, 0..) |val, idx| {
        result[idx] = val;
    }
    break :blk result;
};

// 内存限制配置
// zig build --eval-branch-quota 1000000
