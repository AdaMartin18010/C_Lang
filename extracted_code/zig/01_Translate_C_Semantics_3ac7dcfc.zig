//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 325
// Language: zig
// Block ID: 3ac7dcfc
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码
pub inline fn SQUARE(x: anytype) @TypeOf(x) {
    return (x) * (x);
}

pub inline fn MAX(a: anytype, b: anytype) @TypeOf(a) {
    return if ((a) > (b)) (a) else (b);
}

pub inline fn IS_POWER_OF_TWO(n: anytype) c_int {
    return @intFromBool((((n) & ((n) - 1)) == 0));
}
