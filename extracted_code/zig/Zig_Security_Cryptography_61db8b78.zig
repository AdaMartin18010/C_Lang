//
// Auto-generated from: Zig\Zig_Security_Cryptography.md
// Line: 493
// Language: zig
// Block ID: 61db8b78
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Ed25519 签名 (现代、快速、安全)
pub const Ed25519KeyPair = struct {
    public_key: [32]u8,
    secret_key: [64]u8,  // 扩展私钥格式

    pub fn generate() !Ed25519KeyPair {
        var seed: [32]u8 = undefined;
        crypto.random.bytes(&seed);

        var key_pair: Ed25519KeyPair = undefined;
        try crypto.sign.Ed25519.seedToKeyPair(
            &key_pair.public_key,
            &key_pair.secret_key,
            seed,
        );
        return key_pair;
    }

    pub fn sign(self: Ed25519KeyPair, message: []const u8) [64]u8 {
        var sig: [64]u8 = undefined;
        crypto.sign.Ed25519.sign(&sig, message, self.secret_key, self.public_key);
        return sig;
    }

    pub fn verify(
        public_key: [32]u8,
        message: []const u8,
        signature: [64]u8,
    ) !void {
        try crypto.sign.Ed25519.verify(signature, message, public_key);
    }
};

// 使用示例
pub fn ed25519Example() !void {
    const key_pair = try Ed25519KeyPair.generate();

    const message = "Hello, World!";
    const signature = key_pair.sign(message);

    try Ed25519KeyPair.verify(key_pair.public_key, message, signature);
    std.debug.print("Signature verified!\n", .{});
}
