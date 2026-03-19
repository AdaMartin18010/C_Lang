//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
// Line: 1146
// Language: zig
// Block ID: a2cb2e1d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码: 函数指针调用约定

// 使用 callconv(.C) 指定 System V AMD64 ABI
const Callback = *const fn (i32, i32) callconv(.C) i32;

// 等效于 C 的 callback_t
// 注意: Zig 默认使用 .Unspecified 调用约定
// 与 C 互操作时必须显式使用 callconv(.C)

// 函数接受回调
export fn processWithCallback(
    arr: [*]i32,
    n: usize,
    cb: Callback
) void {
    var i: usize = 0;
    while (i < n) : (i += 1) {
        arr[i] = cb(arr[i], @intCast(i));
    }
}

// 函数返回函数指针
extern fn getCallback(callbackType: i32) ?Callback;

// 函数指针数组
var callbackTable: [10]?Callback = undefined;
