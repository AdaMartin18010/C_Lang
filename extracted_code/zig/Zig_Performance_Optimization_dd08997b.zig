//
// Auto-generated from: Zig\Zig_Performance_Optimization.md
// Line: 293
// Language: zig
// Block ID: dd08997b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 分支预测友好的代码

// ❌ 随机分支
pub fn sumWithBranch(data: []const i32) i64 {
    var sum: i64 = 0;
    for (data) |x| {
        if (x > 0) {  // 随机数据导致分支预测失败
            sum += x;
        }
    }
    return sum;
}

// ✅ 排序后处理 (分支更可预测)
pub fn sumSorted(data: []i32) i64 {
    std.sort.sort(i32, data, {}, comptime std.sort.asc(i32));

    var sum: i64 = 0;
    for (data) |x| {
        if (x <= 0) break;  // 一旦遇到负数就停止
        sum += x;
    }
    return sum;
}

// ✅ 使用条件移动
pub fn conditionalSelect(condition: bool, a: i32, b: i32) i32 {
    // 编译器通常会优化为 CMOV 指令
    return if (condition) a else b;
}
