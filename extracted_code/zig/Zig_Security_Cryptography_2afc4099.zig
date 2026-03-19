//
// Auto-generated from: Zig\Zig_Security_Cryptography.md
// Line: 684
// Language: zig
// Block ID: 2afc4099
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 防止时序攻击的常量时间比较
pub fn constantTimeEql(a: []const u8, b: []const u8) bool {
    if (a.len != b.len) return false;

    var result: u8 = 0;
    for (a, b) |x, y| {
        result |= x ^ y;
    }

    return result == 0;
}

// 使用标准库的 secureZero
pub fn clearSecret(data: []u8) void {
    crypto.utils.secureZero(u8, data);
}
