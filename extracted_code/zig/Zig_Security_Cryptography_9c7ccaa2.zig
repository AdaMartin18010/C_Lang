//
// Auto-generated from: Zig\Zig_Security_Cryptography.md
// Line: 541
// Language: zig
// Block ID: 9c7ccaa2
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// ECDSA P-256 签名
pub const EcdsaP256KeyPair = struct {
    public_key: crypto.ecc.P256.PublicKey,
    secret_key: crypto.ecc.P256.SecretKey,

    pub fn generate() !EcdsaP256KeyPair {
        var seed: [32]u8 = undefined;
        crypto.random.bytes(&seed);

        const key_pair = try crypto.ecc.P256.KeyPair.create(seed);
        return .{
            .public_key = key_pair.public_key,
            .secret_key = key_pair.secret_key,
        };
    }

    pub fn sign(self: EcdsaP256KeyPair, message: []const u8) !crypto.ecc.P256.Signature {
        return try self.secret_key.sign(message, crypto.random);
    }

    pub fn verify(
        self: EcdsaP256KeyPair,
        message: []const u8,
        signature: crypto.ecc.P256.Signature,
    ) !void {
        try self.public_key.verify(message, signature);
    }
};
