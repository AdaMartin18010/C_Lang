//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
// Line: 1737
// Language: zig
// Block ID: 913d00ac
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 常见陷阱
// 陷阱 1：@ptrCast 误用
fn trap_ptr_cast() void {
    var x: i32 = 42;
    const p: *i32 = &x;
    const fp: *f32 = @ptrCast(p);  // 违反严格别名
    fp.* = 3.14;  // 未定义行为！
}

// 陷阱 2：切片越界
fn trap_slice_oob() void {
    var arr: [10]i32 = undefined;
    const slice = arr[0..5];
    // slice[10] = 42;  // Release 模式下未定义行为
}

// 陷阱 3：可选指针解包
fn trap_optional_unwrap() void {
    var maybe_ptr: ?*i32 = null;
    // const ptr = maybe_ptr.?;  // 运行时 panic
    _ = maybe_ptr;
}

// 陷阱 4：C 指针转换
fn trap_c_pointer() void {
    const c_ptr: [*c]i32 = @ptrFromInt(0x1000);  // C 风格指针
    // c_ptr[0] = 42;  // 可能段错误
    _ = c_ptr;
}

// 陷阱 5：Arena 分配器误用
fn trap_arena() !void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    const allocator = arena.allocator();

    const ptr = try allocator.create(i32);
    arena.deinit();  // 所有内存释放
    // ptr.* = 42;  // Use-After-Free!
    _ = ptr;
}
