//
// Auto-generated from: Zig\Zig_Security_Cryptography.md
// Line: 659
// Language: zig
// Block ID: 81882bfa
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// CSPRNG (加密安全伪随机数生成器)
pub fn secureRandom() void {
    var buffer: [32]u8 = undefined;

    // 使用操作系统提供的 CSPRNG
    crypto.random.bytes(&buffer);

    // 或者使用 std.crypto.random
    const Random = @TypeOf(crypto.random);
    var rng = crypto.random;
    const value = rng.int(u64);
    _ = value;
}

// 密钥生成
pub fn generateKey(comptime length: usize) [length]u8 {
    var key: [length]u8 = undefined;
    crypto.random.bytes(&key);
    return key;
}
