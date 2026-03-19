//
// Auto-generated from: Zig\Zig_Security_Cryptography.md
// Line: 781
// Language: zig
// Block ID: ef2129b0
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 安全消息格式: [nonce(12)] [ciphertext] [tag(16)]
pub const SecureMessage = struct {
    const NONCE_LEN = 12;
    const TAG_LEN = 16;

    pub fn encrypt(
        allocator: std.mem.Allocator,
        plaintext: []const u8,
        key: [32]u8,
    ) ![]u8 {
        // 分配输出缓冲区
        const out_len = NONCE_LEN + plaintext.len + TAG_LEN;
        var output = try allocator.alloc(u8, out_len);
        errdefer allocator.free(output);

        // 生成随机 nonce
        var nonce: [NONCE_LEN]u8 = undefined;
        crypto.random.bytes(&nonce);

        // 复制 nonce
        @memcpy(output[0..NONCE_LEN], &nonce);

        // 加密
        var tag: [TAG_LEN]u8 = undefined;
        crypto.aead.chacha_poly.XChaCha20Poly1305.encrypt(
            output[NONCE_LEN..][0..plaintext.len],
            &tag,
            plaintext,
            &[_]u8{},  // 无附加数据
            nonce,
            key,
        );

        // 复制 tag
        @memcpy(output[NONCE_LEN + plaintext.len..][0..TAG_LEN], &tag);

        return output;
    }

    pub fn decrypt(
        allocator: std.mem.Allocator,
        ciphertext: []const u8,
        key: [32]u8,
    ) ![]u8 {
        if (ciphertext.len < NONCE_LEN + TAG_LEN) return error.InvalidMessage;

        const plaintext_len = ciphertext.len - NONCE_LEN - TAG_LEN;
        var plaintext = try allocator.alloc(u8, plaintext_len);
        errdefer allocator.free(plaintext);

        const nonce = ciphertext[0..NONCE_LEN].*;
        const encrypted = ciphertext[NONCE_LEN..][0..plaintext_len];
        const tag = ciphertext[NONCE_LEN + plaintext_len..][0..TAG_LEN].*;

        try crypto.aead.chacha_poly.XChaCha20Poly1305.decrypt(
            plaintext,
            encrypted,
            tag,
            &[_]u8{},
            nonce,
            key,
        );

        return plaintext;
    }
};
