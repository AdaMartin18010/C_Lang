//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
// Line: 943
// Language: zig
// Block ID: 4f19e180
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 越界处理
fn zigOutOfBounds() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    const slice = try allocator.alloc(i32, 10);
    defer allocator.free(slice);

    // 切片访问自动边界检查
    slice[5] = 42;  // OK
    // slice[15] = 42;  // Debug: panic, Release: 未定义行为

    // 使用多指针需要手动管理
    const ptr: [*]i32 = slice.ptr;
    const end = ptr + slice.len;  // one-past-end

    // Zig 不阻止越界指针算术，但使用是不安全的
    const beyond = ptr + 100;
    _ = beyond;
    _ = end;
}
