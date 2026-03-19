//
// Auto-generated from: Zig\Zig_Security_Cryptography.md
// Line: 459
// Language: zig
// Block ID: f964e8d3
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// X25519 密钥交换
pub fn x25519KeyExchange() !void {
    // Alice 生成密钥对
    var alice_private: [32]u8 = undefined;
    var alice_public: [32]u8 = undefined;
    crypto.random.bytes(&alice_private);
    crypto.ecc.X25519.recoverPublicKey(alice_public, alice_private);

    // Bob 生成密钥对
    var bob_private: [32]u8 = undefined;
    var bob_public: [32]u8 = undefined;
    crypto.random.bytes(&bob_private);
    crypto.ecc.X25519.recoverPublicKey(bob_public, bob_private);

    // Alice 计算共享密钥
    var alice_shared: [32]u8 = undefined;
    try crypto.ecc.X25519.scalarmult(alice_shared, alice_private, bob_public);

    // Bob 计算共享密钥
    var bob_shared: [32]u8 = undefined;
    try crypto.ecc.X25519.scalarmult(bob_shared, bob_private, alice_public);

    // 两者应该相同
    std.debug.assert(std.mem.eql(u8, &alice_shared, &bob_shared));
}
