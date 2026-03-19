//
// Auto-generated from: Zig\Zig_Security_Cryptography.md
// Line: 398
// Language: zig
// Block ID: 53864384
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// ChaCha20 流加密
pub fn chacha20Stream() void {
    var key: [32]u8 = undefined;
    var nonce: [12]u8 = undefined;
    crypto.random.bytes(&key);
    crypto.random.bytes(&nonce);

    var cipher = crypto.stream.chacha.ChaCha20IETF.init(key, nonce, 0);

    var plaintext = "Hello, World!".**;
    var ciphertext: [13]u8 = undefined;

    // 加密
    cipher.xor(&ciphertext, &plaintext);

    // 重置并解密
    cipher = crypto.stream.chacha.ChaCha20IETF.init(key, nonce, 0);
    var decrypted: [13]u8 = undefined;
    cipher.xor(&decrypted, &ciphertext);
}
