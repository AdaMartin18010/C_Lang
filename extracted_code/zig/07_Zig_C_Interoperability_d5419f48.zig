//
// Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\07_Zig_C_Interoperability.md
// Line: 97
// Language: zig
// Block ID: d5419f48
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 编译期计算数组长度
fn sum(comptime T: type, arr: []const T) T {
    var total: T = 0;
    for (arr) |item| {
        total += item;
    }
    return total;
}

// 编译期生成代码
fn makeMultiplier(comptime factor: i32) fn (i32) i32 {
    return struct {
        fn multiply(x: i32) i32 {
            return x * factor;  // factor在编译期确定
        }
    }.multiply;
}

const double = makeMultiplier(2);
const triple = makeMultiplier(3);

test "multipliers" {
    try expect(double(5) == 10);
    try expect(triple(5) == 15);
}
