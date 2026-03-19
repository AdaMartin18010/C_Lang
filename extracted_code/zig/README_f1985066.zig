//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\README.md
// Line: 307
// Language: zig
// Block ID: f1985066
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// C 代码:
/// int sum(int count, ...) {
///     va_list args;
///     va_start(args, count);
///     int total = 0;
///     for (int i = 0; i < count; i++) {
///         total += va_arg(args, int);
///     }
///     va_end(args);
///     return total;
/// }

// Zig 使用数组切片或 anytype
pub fn sum(args: []const i32) i32 {
    var total: i32 = 0;
    for (args) |arg| {
        total += arg;
    }
    return total;
}

// 或者使用编译时变参
pub fn sumComptime(comptime args: anytype) i32 {
    var total: i32 = 0;
    inline for (args) |arg| {
        total += arg;
    }
    return total;
}

// 调用: sumComptime(.{1, 2, 3, 4, 5})
