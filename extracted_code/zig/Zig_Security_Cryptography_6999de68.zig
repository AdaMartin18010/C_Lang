//
// Auto-generated from: Zig\Zig_Security_Cryptography.md
// Line: 250
// Language: zig
// Block ID: 6999de68
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const crypto = std.crypto;

// AES-256-GCM 加密
pub fn aesGcmEncrypt(
    allocator: std.mem.Allocator,
    plaintext: []const u8,
    key: [32]u8,
    nonce: [12]u8,
    associated_data: []const u8,
) !struct { ciphertext: []u8, tag: [16]u8 } {
    var ciphertext = try allocator.alloc(u8, plaintext.len);
    errdefer allocator.free(ciphertext);

    var tag: [16]u8 = undefined;

    crypto.aead.aes_gcm.Aes256Gcm.encrypt(
        ciphertext,
        &tag,
        plaintext,
        associated_data,
        nonce,
        key,
    );

    return .{ .ciphertext = ciphertext, .tag = tag };
}

// AES-256-GCM 解密
pub fn aesGcmDecrypt(
    allocator: std.mem.Allocator,
    ciphertext: []const u8,
    key: [32]u8,
    nonce: [12]u8,
    tag: [16]u8,
    associated_data: []const u8,
) ![]u8 {
    var plaintext = try allocator.alloc(u8, ciphertext.len);
    errdefer allocator.free(plaintext);

    try crypto.aead.aes_gcm.Aes256Gcm.decrypt(
        plaintext,
        ciphertext,
        tag,
        associated_data,
        nonce,
        key,
    );

    return plaintext;
}

// 使用示例
pub fn aesExample() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // 随机生成密钥和 nonce
    var key: [32]u8 = undefined;
    var nonce: [12]u8 = undefined;
    crypto.random.bytes(&key);
    crypto.random.bytes(&nonce);

    const message = "Secret message";
    const aad = "additional data";

    // 加密
    const encrypted = try aesGcmEncrypt(
        allocator,
        message,
        key,
        nonce,
        aad,
    );
    defer allocator.free(encrypted.ciphertext);

    // 解密
    const decrypted = try aesGcmDecrypt(
        allocator,
        encrypted.ciphertext,
        key,
        nonce,
        encrypted.tag,
        aad,
    );
    defer allocator.free(decrypted);

    std.debug.print("Decrypted: {s}\n", .{decrypted});
}
