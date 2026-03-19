//
// Auto-generated from: Zig\formal_models\README.md
// Line: 377
// Language: zig
// Block ID: 7194ddde
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 使用标准库进行运行时验证

const std = @import("std");

// 前置条件检查
fn safe_divide(a: i32, b: i32) i32 {
    std.debug.assert(b != 0);  // 运行时断言
    return @divTrunc(a, b);
}

// 不变量检查
const BoundedInt = struct {
    value: i32,
    min: i32,
    max: i32,

    pub fn init(v: i32, min: i32, max: i32) !BoundedInt {
        if (v < min or v > max) return error.OutOfRange;
        return .{ .value = v, .min = min, .max = max };
    }

    pub fn add(self: BoundedInt, other: i32) !BoundedInt {
        const result = std.math.add(i32, self.value, other) catch
            return error.Overflow;
        return init(result, self.min, self.max);
    }
};
