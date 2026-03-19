//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\01_Nullptr_vs_Optional_Pointer.md
// Line: 509
// Language: zig
// Block ID: d8109dbe
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 空指针解引用防护
const std = @import("std");

// 机制 1: 编译时类型分离
fn compile_time_protection() void {
    var maybe: ?*i32 = null;

    // 以下编译错误:
    // _ = maybe.*;  // error: optional type '?*i32' cannot be dereferenced

    // 必须显式处理
    if (maybe) |ptr| {
        _ = ptr.*;  // OK
    }
}

// 机制 2: 强制检查语法
fn mandatory_check_patterns(maybe: ?*i32) void {
    // 模式 1: if-let
    if (maybe) |ptr| {
        // ptr 保证非空
        ptr.* = 42;
    } else {
        // 处理 null
    }

    // 模式 2: orelse
    var default: i32 = 0;
    const ptr = maybe orelse &default;
    ptr.* = 42;  // 安全，指向 default

    // 模式 3: orelse return
    const ptr2 = maybe orelse return;
    ptr2.* = 42;  // 安全，已经返回了
}

// 机制 3: 非空指针类型保证
fn nonnull_guarantee() void {
    var value: i32 = 42;
    const ptr: *i32 = &value;

    // ptr 类型保证非空，可以直接解引用
    ptr.* = 100;
}
