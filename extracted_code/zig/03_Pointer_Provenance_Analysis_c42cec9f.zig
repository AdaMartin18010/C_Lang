//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
// Line: 1421
// Language: zig
// Block ID: c42cec9f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig Use-After-Free 防护
fn zigUseAfterFree() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    const ptr = try allocator.create(i32);
    ptr.* = 42;

    allocator.destroy(ptr);
    // ptr 现在无效

    // GPA 可能检测到此非法访问
    // const x = ptr.*;  // 可能的错误检测
}

// 安全的模式：使用可选指针
fn zigSafePointer() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var maybe_ptr: ?*i32 = try allocator.create(i32);

    if (maybe_ptr) |ptr| {
        ptr.* = 42;
        allocator.destroy(ptr);
        maybe_ptr = null;  // 清空引用
    }

    // 现在安全地检查
    if (maybe_ptr) |ptr| {
        _ = ptr.*;  // 不会执行
    }
}

// Arena 分配器避免 UAF
fn zigArenaAllocator() !void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    const allocator = arena.allocator();

    // 所有分配共享生命周期
    const p1 = try allocator.create(i32);
    const p2 = try allocator.create(i32);

    p1.* = 1;
    p2.* = 2;

    // 无需单独释放，arena.deinit() 统一释放
    _ = p1;
    _ = p2;
}
