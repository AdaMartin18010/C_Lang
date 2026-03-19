//
// Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\07_Zig_C_Interoperability.md
// Line: 861
// Language: zig
// Block ID: 595c0f5e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

陷阱1: 调用约定不匹配
// 错误
fn my_func(x: i32) i32 { ... }  // Zig调用约定

// 正确
export fn my_func(x: c_int) c_int callconv(.C) { ... }

陷阱2: 空指针处理
// Zig中非空指针不能为null
var p: *i32 = null;  // 错误！
var opt: ?*i32 = null;  // 正确

// 从C接收指针时要检查
const ptr: ?*c.FILE = c.fopen("file", "r");
if (ptr) |file| { ... }

陷阱3: 内存布局假设
// 不要假设C结构体布局
// 使用extern struct确保兼容
const MyStruct = extern struct {
    x: c_int,
    y: c_float,
};

陷阱4: 字符串处理
// C字符串需要转换为Zig切片
const c_str: [*c]const u8 = c.get_string();
const zig_slice = std.mem.span(c_str);
