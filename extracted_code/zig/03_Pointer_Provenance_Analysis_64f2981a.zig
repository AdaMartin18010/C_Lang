//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
// Line: 1159
// Language: zig
// Block ID: 64f2981a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 的优化控制策略

// 策略 1：显式内存序
fn explicitMemoryOrdering() void {
    var data: i32 = 0;
    var flag = std.atomic.Value(bool).init(false);

    // 生产者
    data = 42;
    flag.store(true, .Release);  // 确保 data 写入先发生

    // 消费者
    while (!flag.load(.Acquire)) {}
    const value = data;  // 保证看到 42
    _ = value;
}

// 策略 2：使用 black_box 防止过度优化
fn preventOverOptimization() void {
    var x: i32 = 42;
    const p: *i32 = &x;

    p.* = 1;
    std.mem.doNotOptimizeAway(p);  // 告诉编译器 p 有副作用
    p.* = 2;

    const val = p.*;
    std.debug.print("{d}\n", .{val});
}

// 策略 3：使用正确的类型
fn correctTypeUsage() void {
    var bytes: [4]u8 = .{ 0, 0, 0, 0 };

    // 错误：直接类型双关
    // const f: *f32 = @ptrCast(&bytes);

    // 正确：使用 bytesToValue
    const f = std.mem.bytesToValue(f32, &bytes);
    _ = f;
}
