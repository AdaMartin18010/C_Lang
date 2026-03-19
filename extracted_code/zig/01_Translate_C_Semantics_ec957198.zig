//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 832
// Language: zig
// Block ID: ec957198
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码
pub extern fn printf(format: [*c]const u8, ...) c_int;
pub extern fn sprintf(str: [*c]u8, format: [*c]const u8, ...) c_int;
pub extern fn log_message(level: c_int, fmt: [*c]const u8, ...) void;

// Zig 中可变参数函数的使用需要小心
// 必须使用 @cVaStart, @cVaArg, @cVaEnd 宏

// 示例：包装 printf
pub fn zigPrintf(comptime fmt: []const u8, args: anytype) c_int {
    const cfmt = std.fmt.comptimePrint(fmt, args);
    return printf(cfmt.ptr);
}
