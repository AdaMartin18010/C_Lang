//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 285
// Language: zig
// Block ID: 384d3dd8
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码 - 数据验证模块（首个迁移的模块）
// file: validation.zig

const std = @import("std");

pub const ValidationError = error{
    EmptyInput,
    TooLong,
    InvalidCharacters,
    NullByteDetected,
};

pub fn validateInput(data: []const u8) ValidationError!void {
    // 安全检查 1: 空输入
    if (data.len == 0) {
        return ValidationError.EmptyInput;
    }

    // 安全检查 2: 长度限制
    if (data.len > 1024) {
        return ValidationError.TooLong;
    }

    // 安全检查 3: 无效字符检查
    for (data) |byte| {
        if (byte == 0) {
            return ValidationError.NullByteDetected;
        }
        if (!std.ascii.isPrint(byte) and !std.ascii.isWhitespace(byte)) {
            return ValidationError.InvalidCharacters;
        }
    }
}

// C 兼容接口
export fn zig_validate_input(data: [*c]const u8, len: usize) c_int {
    if (data == null) return 0;

    const slice = data[0..len];
    validateInput(slice) catch return 0;
    return 1;
}
