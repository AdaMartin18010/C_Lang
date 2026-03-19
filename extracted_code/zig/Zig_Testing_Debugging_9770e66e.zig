//
// Auto-generated from: Zig\Zig_Testing_Debugging.md
// Line: 506
// Language: zig
// Block ID: 9770e66e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// fuzz_test.zig

const std = @import("std");
const testing = std.testing;

// 被测试的解析函数
fn parseNumber(input: []const u8) !i32 {
    if (input.len == 0) return error.EmptyInput;

    var result: i32 = 0;
    var negative = false;
    var i: usize = 0;

    if (input[0] == '-') {
        negative = true;
        i += 1;
    }

    while (i < input.len) : (i += 1) {
        const c = input[i];
        if (c < '0' or c > '9') return error.InvalidCharacter;

        const digit = c - '0';

        // 检查溢出
        if (result > (@import("std").math.maxInt(i32) - digit) / 10) {
            return error.Overflow;
        }

        result = result * 10 + digit;
    }

    return if (negative) -result else result;
}

// 模糊测试入口
export fn zig_fuzz_init() void {}

export fn zig_fuzz_test(data: [*]const u8, len: usize) void {
    const input = data[0..len];

    // 尝试解析 - 不应崩溃
    _ = parseNumber(input) catch {};
}

// 手动模糊测试
test "fuzz with random inputs" {
    var prng = std.rand.DefaultPrng.init(0);
    const random = prng.random();

    var i: usize = 0;
    while (i < 10000) : (i += 1) {
        const len = random.uintLessThan(usize, 20);
        var input: [20]u8 = undefined;

        random.bytes(input[0..len]);

        // 不应崩溃
        _ = parseNumber(input[0..len]) catch {};
    }
}
