//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 1301
// Language: zig
// Block ID: 76152c07
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码
pub extern fn get_name() [*c]const u8;
pub extern fn set_name(name: [*c]const u8) void;
pub extern fn get_buffer() [*c]u8;
pub extern fn process_string(str: [*c]u8) void;
pub extern fn handle_wide(wstr: [*c]c_wchar_t) void;

// Zig 中更安全的字符串处理
pub fn getName() [:0]const u8 {
    const ptr = get_name();
    return std.mem.span(ptr);  // 转换为 Zig 风格切片
}

pub fn setName(name: []const u8) void {
    const c_name = std.cstr.addNullByte(allocator, name) catch unreachable;
    defer allocator.free(c_name);
    set_name(c_name.ptr);
}
