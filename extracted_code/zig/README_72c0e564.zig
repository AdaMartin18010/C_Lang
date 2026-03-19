//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\README.md
// Line: 258
// Language: zig
// Block ID: 72c0e564
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

test "GPA detects double free" {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    const ptr = try allocator.alloc(u8, 100);
    allocator.free(ptr);
    // allocator.free(ptr); // 取消注释将导致双重释放错误
}

test "GPA detects use after free" {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    const ptr = try allocator.alloc(u8, 100);
    allocator.free(ptr);
    // ptr[0] = 1; // 错误：使用已释放内存
}
