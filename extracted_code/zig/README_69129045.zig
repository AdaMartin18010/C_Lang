//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\README.md
// Line: 251
// Language: zig
// Block ID: 69129045
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 指针类型详解
const c = @cImport(@cInclude("stdlib.h"));

pub fn pointerDemo() void {
    // C 风格单指针
    var ptr: *i32 = undefined;

    // 可空指针 (对应 C 中可能为 NULL 的指针)
    var maybe_ptr: ?*i32 = null;

    // 从可空指针获取非空指针
    if (maybe_ptr) |non_null_ptr| {
        // 在这里 non_null_ptr 是 *i32 类型
        non_null_ptr.* = 42;
    }

    // malloc 返回的可空指针
    var mem: ?*anyopaque = c.malloc(100);
    defer if (mem) |m| c.free(m);
}
