//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\01_Nullptr_vs_Optional_Pointer.md
// Line: 307
// Language: zig
// Block ID: fd16950b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 可选指针语法完整指南

// 1. 类型声明
var p1: ?*i32 = null;           // 可选的可变指针
var p2: ?*const i32 = null;     // 可选的只读指针
var p3: ?[*]i32 = null;         // 可选的 C 风格多元素指针
var p4: ?*anyopaque = null;     // 可选的通用指针

// 2. 空值检查语法
fn null_check_patterns(maybe_ptr: ?*i32) void {
    // 模式 1: if 解构 (推荐)
    if (maybe_ptr) |ptr| {
        // ptr 是 *i32 (非空)，可直接解引用
        const value = ptr.*;
        _ = value;
    } else {
        // 处理 null 情况
    }

    // 模式 2: orelse (提供默认值)
    var default: i32 = 0;
    const ptr = maybe_ptr orelse &default;

    // 模式 3: orelse 返回/继续
    const ptr2 = maybe_ptr orelse return;  // 早返回

    // 模式 4: 强制解包 (.?)
    // const ptr3 = maybe_ptr.?;  // panic if null
}
