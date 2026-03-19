//
// Auto-generated from: Zig\Zig_Security_Cryptography.md
// Line: 206
// Language: zig
// Block ID: fc6f57f0
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Argon2 密码哈希
pub fn argon2Example() !void {
    const password = "user_password";
    const salt = "random_salt_16bytes";

    var hash: [32]u8 = undefined;

    // Argon2id 参数: m=65536, t=3, p=4
    try crypto.pwhash.argon2.argon2id(
        &hash,
        password,
        salt,
        .{
            .t = 3,       // 迭代次数
            .m = 65536,   // 内存使用 (64MB)
            .p = 4,       // 并行度
        },
        .{}  // 变体
    );
}

// PBKDF2 (兼容性)
pub fn pbkdf2Example() void {
    const password = "password";
    const salt = "salt";

    var derived_key: [32]u8 = undefined;
    crypto.pwhash.pbkdf2.pbkdf2(
        &derived_key,
        password,
        salt,
        100000,  // 迭代次数
        crypto.hash.sha2.HmacSha256,
    );
}
