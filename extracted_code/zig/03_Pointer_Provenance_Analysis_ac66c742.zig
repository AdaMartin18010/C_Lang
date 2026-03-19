//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
// Line: 441
// Language: zig
// Block ID: ac66c742
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 来源验证示例
fn compileTimeValidation() void {
    var arr: [10]i32 = undefined;

    // 编译时检查切片边界
    const slice = arr[0..5];  // OK
    _ = slice;
    // const bad_slice = arr[0..20];  // 编译错误：索引越界

    // 编译时检查指针对齐
    const aligned: *align(4) i32 = &arr[0];  // OK
    _ = aligned;
}

fn runtimeValidation() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer {
        const leaked = gpa.deinit();
        if (leaked == .leak) std.log.err("Memory leak!", .{});
    }
    const allocator = gpa.allocator();

    // 分配切片
    const slice = try allocator.alloc(i32, 10);
    defer allocator.free(slice);

    // 运行时边界检查（Debug 模式）
    slice[5] = 42;  // OK
    // slice[15] = 42;  // 运行时 panic：索引越界

    // 指针失效后使用检测（GPA 提供）
    const ptr = slice.ptr;
    allocator.free(slice);
    // ptr[0] = 42;  // Use-after-free！GPA 可能检测
    _ = ptr;
}
