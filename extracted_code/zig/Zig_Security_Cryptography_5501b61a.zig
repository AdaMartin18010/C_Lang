//
// Auto-generated from: Zig\Zig_Security_Cryptography.md
// Line: 344
// Language: zig
// Block ID: 5501b61a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// ChaCha20-Poly1305 (现代高性能 AEAD)
pub fn chacha20Poly1305Encrypt(
    allocator: std.mem.Allocator,
    plaintext: []const u8,
    key: [32]u8,
    nonce: [12]u8,
    associated_data: []const u8,
) !struct { ciphertext: []u8, tag: [16]u8 } {
    var ciphertext = try allocator.alloc(u8, plaintext.len);
    errdefer allocator.free(ciphertext);

    var tag: [16]u8 = undefined;

    crypto.aead.chacha_poly.ChaCha20Poly1305.encrypt(
        ciphertext,
        &tag,
        plaintext,
        associated_data,
        nonce,
        key,
    );

    return .{ .ciphertext = ciphertext, .tag = tag };
}

// XChaCha20-Poly1305 (扩展 nonce，更安全)
pub fn xchacha20Encrypt(
    allocator: std.mem.Allocator,
    plaintext: []const u8,
    key: [32]u8,
    nonce: [24]u8,
    associated_data: []const u8,
) !struct { ciphertext: []u8, tag: [16]u8 } {
    var ciphertext = try allocator.alloc(u8, plaintext.len);
    errdefer allocator.free(ciphertext);

    var tag: [16]u8 = undefined;

    crypto.aead.chacha_poly.XChaCha20Poly1305.encrypt(
        ciphertext,
        &tag,
        plaintext,
        associated_data,
        nonce,
        key,
    );

    return .{ .ciphertext = ciphertext, .tag = tag };
}
