//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\README.md
// Line: 271
// Language: zig
// Block ID: d24ea344
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// C 陷阱: 小于 int 的类型运算时会提升
/// char a = 100, b = 100;
/// char c = a + b;  // 可能溢出或符号扩展

// Zig 明确指定类型
var a: i8 = 100;
var b: i8 = 100;
var c = @as(i8, @intCast(a +% b));  // 明确使用回绕加法

// 或者更安全的做法
var c_safe = a +| b;  // 饱和加法
if (a > std.math.maxInt(i8) - b) {
    return error.Overflow;
}
