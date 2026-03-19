//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\02_CImport_Correctness.md
// Line: 272
// Language: zig
// Block ID: c68decc6
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

// 注意: ARRAY_SIZE 不能简单翻译，因为 sizeof 在 Zig 中不同
// 可能需要手动处理
