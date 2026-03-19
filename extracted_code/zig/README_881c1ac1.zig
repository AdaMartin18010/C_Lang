//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\README.md
// Line: 224
// Language: zig
// Block ID: 881c1ac1
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// C 代码:
/// #define MAX(a, b) ((a) > (b) ? (a) : (b))
/// #define SQUARE(x) ((x) * (x))
/// #define DEBUG_PRINT(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)

// Zig 翻译 - 使用泛型和编译期特性
pub fn max(comptime T: type, a: T, b: T) T {
    return if (a > b) a else b;
}

pub fn square(x: anytype) @TypeOf(x) {
    return x * x;
}

pub fn debugPrint(comptime fmt: []const u8, args: anytype) void {
    if (@import("builtin").mode == .Debug) {
        std.debug.print("[DEBUG] " ++ fmt ++ "\n", args);
    }
}

/// 复杂宏 - 条件编译
/// #ifdef ENABLE_FEATURE
/// #define FEATURE_API(x) process_feature(x)
/// #else
/// #define FEATURE_API(x) (x)
/// #endif

// Zig 使用编译期布尔
const enable_feature = @import("config").enable_feature;

pub fn featureApi(x: i32) i32 {
    if (enable_feature) {
        return processFeature(x);
    }
    return x;
}

fn processFeature(x: i32) i32 {
    return x * 2;
}
