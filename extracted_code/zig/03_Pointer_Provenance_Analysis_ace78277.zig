//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
// Line: 1042
// Language: zig
// Block ID: ace78277
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 整数↔指针转换
fn zigIntPointerConversion() void {
    var x: i32 = 42;
    const p: *i32 = &x;

    // 指针 → 整数
    const addr = @ptrToInt(p);

    // 整数 → 指针
    const q: *i32 = @intToPtr(*i32, addr);

    std.debug.assert(q.* == 42);

    // Zig 的类型系统强制显式转换
    // 没有隐式 int → ptr 或 ptr → int
}

// Zig 的 @intToPtr 注意事项
fn zigIntToPtrSafety() void {
    // 随机地址转换
    const random_addr: usize = 0x1000;
    const ptr = @intToPtr(*i32, random_addr);
    _ = ptr;
    // ptr.* = 42;  // 可能导致段错误！

    // 安全的方式：验证地址有效性
    // 需要与操作系统/分配器配合
}

// 使用可选指针处理可能无效的地址
fn zigSafePointerHandling() void {
    const addr: usize = @ptrToInt(&global_var);

    // 验证地址范围
    const maybe_ptr: ?*i32 = if (addr > 0x1000)
        @intToPtr(*i32, addr)
    else
        null;

    if (maybe_ptr) |ptr| {
        std.debug.assert(ptr.* == global_var);
    }
}

var global_var: i32 = 42;
