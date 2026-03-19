//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 1127
// Language: zig
// Block ID: 4203b347
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

fn aes_gcm_encryption() !void {
    const allocator = std.heap.page_allocator;

    // AES-256-GCM
    const key = "0123456789abcdef0123456789abcdef";  // 32字节
    const nonce = "unique_nonce_123";  // 12字节
    const plaintext = "Secret data to encrypt";
    const associated_data = "additional authenticated data";

    // 加密
    const ciphertext = try std.crypto.aes.aes256gcm.encrypt(
        allocator,
        plaintext,
        associated_data,
        nonce,
        key
    );
    defer allocator.free(ciphertext);

    std.log.info("Ciphertext: {s}", .{std.fmt.bytesToHex(ciphertext, .lower)});

    // 解密
    const decrypted = try std.crypto.aes.aes256gcm.decrypt(
        allocator,
        ciphertext,
        associated_data,
        nonce,
        key
    );
    defer allocator.free(decrypted);

    std.log.info("Decrypted: {s}", .{decrypted});
}
