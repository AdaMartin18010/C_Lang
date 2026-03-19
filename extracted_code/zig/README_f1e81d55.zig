//
// Auto-generated from: Zig\matrices\README.md
// Line: 157
// Language: zig
// Block ID: f1e81d55
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 显式错误处理 + comptime
fn parseNumber(comptime T: type, s: []const u8) !T {
    return std.fmt.parseInt(T, s, 10);
}

const n = try parseNumber(u32, "42");
