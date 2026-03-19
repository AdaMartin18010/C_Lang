//
// Auto-generated from: Zig\Zig_Performance_Optimization.md
// Line: 701
// Language: zig
// Block ID: a6e02764
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 强制内联
inline fn addInline(a: i32, b: i32) i32 {
    return a + b;
}

// 循环展开提示
pub fn sumUnrolled(data: []const i32) i64 {
    var sum: i64 = 0;
    var i: usize = 0;

    // 手动展开
    while (i + 4 <= data.len) : (i += 4) {
        sum += data[i];
        sum += data[i + 1];
        sum += data[i + 2];
        sum += data[i + 3];
    }

    // 剩余
    while (i < data.len) : (i += 1) {
        sum += data[i];
    }

    return sum;
}

// 使用 comptime 展开
pub fn sumComptimeUnroll(comptime unroll: usize, data: []const i32) i64 {
    var sum: i64 = 0;
    var i: usize = 0;

    while (i + unroll <= data.len) : (i += unroll) {
        comptime var j: usize = 0;
        inline while (j < unroll) : (j += 1) {
            sum += data[i + j];
        }
    }

    while (i < data.len) : (i += 1) {
        sum += data[i];
    }

    return sum;
}
