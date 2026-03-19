//
// Auto-generated from: Zig\Zig_Security_Cryptography.md
// Line: 172
// Language: zig
// Block ID: 6c845875
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// HMAC-SHA256
pub fn hmacExample() void {
    const key = "secret_key";
    const message = "authenticated message";

    var mac: [32]u8 = undefined;
    crypto.auth.hmac.sha2.HmacSha256.create(&mac, message, key);

    // 验证
    var verified_mac: [32]u8 = undefined;
    crypto.auth.hmac.sha2.HmacSha256.create(&verified_mac, message, key);

    if (std.mem.eql(u8, &mac, &verified_mac)) {
        std.debug.print("MAC verified!\n", .{});
    }
}

// 密钥派生 (HKDF)
pub fn hkdfExample() void {
    const ikm = "input key material";
    const salt = "salt value";
    const info = "application info";

    var prk: [32]u8 = undefined;
    var okm: [64]u8 = undefined;

    crypto.kdf.hkdf.HkdfSha256.extract(&prk, salt, ikm);
    crypto.kdf.hkdf.HkdfSha256.expand(&okm, info, prk);
}
