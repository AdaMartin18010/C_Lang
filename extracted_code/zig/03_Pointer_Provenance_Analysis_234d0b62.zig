//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
// Line: 1511
// Language: zig
// Block ID: 234d0b62
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 防止悬挂指针
// 编译时检查生命周期
fn zigLifetimeCheck() void {
    var p: ?*i32 = null;

    {
        var local: i32 = 42;
        p = &local;  // 编译错误：指向局部变量的指针逃逸
    }

    _ = p;
}

// 重新分配的正确处理
fn zigRealloc() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var slice = try allocator.alloc(i32, 10);

    // realloc 返回新切片，旧切片可能失效
    slice = try allocator.realloc(slice, 20);

    // 现在安全使用 slice
    slice[0] = 42;
}

// 使用 ArrayList 自动管理
fn zigArrayList() !void {
    var list = std.ArrayList(i32).init(std.heap.page_allocator);
    defer list.deinit();

    try list.append(1);
    try list.append(2);

    // 扩容自动处理，不会有悬挂指针
    const elem = &list.items[0];  // 注意：追加后可能失效！
    _ = elem;
}
