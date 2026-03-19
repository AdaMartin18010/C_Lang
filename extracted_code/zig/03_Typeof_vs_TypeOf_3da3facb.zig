//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
// Line: 1832
// Language: zig
// Block ID: 3da3facb
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 迁移后的 Zig 代码

// 方式 1: 使用 comptime 类型参数
fn max(comptime T: type, a: T, b: T) T {
    return if (a > b) a else b;
}

// 方式 2: 使用 @TypeOf 推断
fn maxInferred(a: anytype, b: @TypeOf(a)) @TypeOf(a) {
    return if (a > b) a else b;
}

// 使用
const m1 = max(i32, 10, 20);
const m2 = maxInferred(3.14, 2.71);
