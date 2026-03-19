//
// Auto-generated from: Zig\Zig_C_Interoperability.md
// Line: 265
// Language: zig
// Block ID: 8dc3e0ab
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// C 和 Zig 类型转换

const c = @cImport({
    @cInclude("example.h");
});

// 指针转换
fn zig_to_c_ptr() void {
    var zig_array = [_]u8{ 1, 2, 3, 4 };

    // Zig 指针转 C 指针
    const c_ptr: [*c]u8 = &zig_array;

    // C 指针转 Zig 指针
    const zig_ptr: [*]u8 = c_ptr;

    // 可选指针
    const maybe_c_ptr: ?[*c]u8 = if (zig_array.len > 0) &zig_array else null;
}

// 字符串转换
fn string_conversions() void {
    // Zig 字符串转 C 字符串
    const zig_str: []const u8 = "hello";
    const c_str: [*c]const u8 = zig_str.ptr;

    // C 字符串转 Zig 字符串
    const c_string: [*c]const u8 = c.get_string();
    const zig_string = std.mem.span(c_string);  // 以 null 结尾的 C 字符串

    // 创建 C 兼容字符串
    var buffer: [256:0]u8 = undefined;
    const c_compatible = std.fmt.bufPrintZ(&buffer, "Hello {}", .{42}) catch unreachable;
}

// 数组和切片
fn array_conversions() void {
    // C 数组
    var c_array: [10]c_int = undefined;

    // 转 Zig 切片
    const zig_slice: []c_int = &c_array;

    // 从不定长 C 数组创建切片
    const c_ptr: [*c]c_int = c.get_array(&c_array.len);
    const zig_slice2: []c_int = c_ptr[0..c_array.len];
}

// 结构体转换
const CPoint = extern struct {
    x: c_int,
    y: c_int,
};

fn struct_conversions() void {
    // 创建 C 兼容结构体
    const point = CPoint{ .x = 10, .y = 20 };

    // 传递给 C 函数
    c.process_point(&point);

    // 从 C 获取结构体
    var received: CPoint = undefined;
    c.get_point(&received);
}
