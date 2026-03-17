# Zig 安全与密码学：从哈希到加密通信

> **定位**: Zig 核心 / 安全加密 | **难度**: ⭐⭐⭐⭐⭐ | **目标**: 掌握 Zig 安全编程

---

## 📋 目录

- [Zig 安全与密码学：从哈希到加密通信](#zig-安全与密码学从哈希到加密通信)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、安全编程概述](#一安全编程概述)
  - [二、哈希算法](#二哈希算法)
    - [2.1 标准库哈希](#21-标准库哈希)
    - [2.2 HMAC (哈希消息认证码)](#22-hmac-哈希消息认证码)
    - [2.3 密码哈希 (Argon2)](#23-密码哈希-argon2)
  - [三、对称加密](#三对称加密)
    - [3.1 AES-GCM](#31-aes-gcm)
    - [3.2 ChaCha20-Poly1305](#32-chacha20-poly1305)
    - [3.3 流加密 (ChaCha20)](#33-流加密-chacha20)
  - [四、非对称加密](#四非对称加密)
    - [4.1 RSA](#41-rsa)
    - [4.2 ECDH (椭圆曲线 Diffie-Hellman)](#42-ecdh-椭圆曲线-diffie-hellman)
  - [五、数字签名](#五数字签名)
    - [5.1 Ed25519](#51-ed25519)
    - [5.2 ECDSA](#52-ecdsa)
  - [六、TLS/SSL 实现](#六tlsssl-实现)
    - [6.1 TLS 客户端](#61-tls-客户端)
    - [6.2 证书验证](#62-证书验证)
  - [七、安全最佳实践](#七安全最佳实践)
    - [7.1 安全随机数](#71-安全随机数)
    - [7.2 常量时间操作](#72-常量时间操作)
    - [7.3 内存安全](#73-内存安全)
  - [八、实战：安全通信协议](#八实战安全通信协议)
    - [8.1 噪声协议框架](#81-噪声协议框架)
    - [8.2 安全消息协议](#82-安全消息协议)

---


---

## 📑 目录

- [Zig 安全与密码学：从哈希到加密通信](#zig-安全与密码学从哈希到加密通信)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、安全编程概述](#一安全编程概述)
  - [二、哈希算法](#二哈希算法)
    - [2.1 标准库哈希](#21-标准库哈希)
    - [2.2 HMAC (哈希消息认证码)](#22-hmac-哈希消息认证码)
    - [2.3 密码哈希 (Argon2)](#23-密码哈希-argon2)
  - [三、对称加密](#三对称加密)
    - [3.1 AES-GCM](#31-aes-gcm)
    - [3.2 ChaCha20-Poly1305](#32-chacha20-poly1305)
    - [3.3 流加密 (ChaCha20)](#33-流加密-chacha20)
  - [四、非对称加密](#四非对称加密)
    - [4.1 RSA](#41-rsa)
    - [4.2 ECDH (椭圆曲线 Diffie-Hellman)](#42-ecdh-椭圆曲线-diffie-hellman)
  - [五、数字签名](#五数字签名)
    - [5.1 Ed25519](#51-ed25519)
    - [5.2 ECDSA](#52-ecdsa)
  - [六、TLS/SSL 实现](#六tlsssl-实现)
    - [6.1 TLS 客户端](#61-tls-客户端)
    - [6.2 证书验证](#62-证书验证)
  - [七、安全最佳实践](#七安全最佳实践)
    - [7.1 安全随机数](#71-安全随机数)
    - [7.2 常量时间操作](#72-常量时间操作)
    - [7.3 内存安全](#73-内存安全)
  - [八、实战：安全通信协议](#八实战安全通信协议)
    - [8.1 噪声协议框架](#81-噪声协议框架)
    - [8.2 安全消息协议](#82-安全消息协议)


---

## 一、安全编程概述

```
┌─────────────────────────────────────────────────────────────────────┐
│                    Zig 安全编程层级                                  │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  应用层                                                               │
│  ├── 输入验证与清理                                                   │
│  ├── 安全会话管理                                                     │
│  └── 访问控制                                                        │
│                                                                      │
│  协议层                                                               │
│  ├── TLS/SSL 握手                                                    │
│  ├── 证书验证                                                        │
│  └── 密钥交换                                                        │
│                                                                      │
│  加密层                                                               │
│  ├── 对称加密 (AES/ChaCha20)                                         │
│  ├── 非对称加密 (RSA/ECC)                                            │
│  ├── 数字签名 (Ed25519/ECDSA)                                        │
│  └── 密钥派生 (Argon2/PBKDF2)                                        │
│                                                                      │
│  基础层                                                               │
│  ├── 哈希函数 (SHA256/BLAKE3)                                        │
│  ├── MAC/HMAC                                                        │
│  ├── 随机数生成                                                      │
│  └── 常量时间操作                                                    │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 二、哈希算法

### 2.1 标准库哈希

```zig
const std = @import("std");
const crypto = std.crypto;

// SHA-256
pub fn sha256Example() void {
    var hash: [32]u8 = undefined;
    const message = "Hello, Zig!";

    crypto.hash.sha2.Sha256.hash(message, &hash, .{});

    // 打印哈希值
    std.debug.print("SHA-256: ", .{});
    for (hash) |byte| {
        std.debug.print("{x:0>2}", .{byte});
    }
    std.debug.print("\n", .{});
}

// SHA-3 / Keccak
pub fn sha3Example() void {
    var hash: [32]u8 = undefined;
    const message = "Hello, Zig!";

    crypto.hash.sha3.Sha3_256.hash(message, &hash, .{});
}

// BLAKE3 (高性能)
pub fn blake3Example() void {
    var hash: [32]u8 = undefined;
    const message = "Hello, Zig!";

    crypto.hash.Blake3.hash(message, &hash, .{});
}

// 增量哈希
pub fn incrementalHash() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var sha256 = crypto.hash.sha2.Sha256.init(.{});

    // 分块更新
    sha256.update("Hello, ");
    sha256.update("World");
    sha256.update("!");

    var hash: [32]u8 = undefined;
    sha256.final(&hash);

    _ = allocator;
}
```

### 2.2 HMAC (哈希消息认证码)

```zig
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
```

### 2.3 密码哈希 (Argon2)

```zig
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
```

---

## 三、对称加密

### 3.1 AES-GCM

```zig
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
```

### 3.2 ChaCha20-Poly1305

```zig
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
```

### 3.3 流加密 (ChaCha20)

```zig
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
```

---

## 四、非对称加密

### 4.1 RSA

```zig
// RSA 密钥生成与操作
const crypto = std.crypto;

pub const RsaKeyPair = struct {
    public_key: [256]u8,   // 2048-bit RSA
    private_key: [256 * 5]u8,  // 包含所有 RSA 组件

    pub fn generate(allocator: std.mem.Allocator) !RsaKeyPair {
        // 使用标准库或外部库生成 RSA 密钥对
        // 注意: Zig 标准库目前不内置 RSA 生成，需要使用外部实现
        _ = allocator;
        @panic("RSA generation requires external implementation");
    }
};

// RSA-OAEP 加密
pub fn rsaEncrypt(
    allocator: std.mem.Allocator,
    message: []const u8,
    public_key: []const u8,
) ![]u8 {
    // PKCS#1 v2.1 OAEP 加密
    _ = allocator;
    _ = message;
    _ = public_key;
    @panic("RSA implementation requires external library");
}
```

### 4.2 ECDH (椭圆曲线 Diffie-Hellman)

```zig
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
```

---

## 五、数字签名

### 5.1 Ed25519

```zig
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
```

### 5.2 ECDSA

```zig
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
```

---

## 六、TLS/SSL 实现

### 6.1 TLS 客户端

```zig
const std = @import("std");
const crypto = std.crypto;
const tls = std.crypto.tls;

pub const TlsClient = struct {
    stream: std.net.tcp.Connection,
    cipher_suite: tls.CipherSuite,
    master_secret: [48]u8,

    pub fn connect(allocator: std.mem.Allocator, host: []const u8, port: u16) !TlsClient {
        // 解析地址
        const address = try std.net.Address.parseIp(host, port);
        const stream = try std.net.tcp.Connection.connect(address);

        // TLS 握手 (简化)
        // 实际实现需要完整的 TLS 1.3 握手协议
        _ = allocator;

        return .{
            .stream = stream,
            .cipher_suite = .TLS_AES_256_GCM_SHA384,
            .master_secret = undefined,
        };
    }

    pub fn send(self: TlsClient, data: []const u8) !void {
        // 加密并发送
        _ = self;
        _ = data;
    }

    pub fn receive(self: TlsClient, buffer: []u8) !usize {
        // 接收并解密
        _ = self;
        _ = buffer;
        return 0;
    }

    pub fn close(self: TlsClient) void {
        // 发送 close_notify
        self.stream.close();
    }
};
```

### 6.2 证书验证

```zig
// X.509 证书解析与验证
pub const Certificate = struct {
    raw: []const u8,
    subject: []const u8,
    issuer: []const u8,
    public_key: []const u8,
    not_before: i64,
    not_after: i64,

    pub fn parse(der: []const u8) !Certificate {
        // X.509 DER 解析
        _ = der;
        @panic("X.509 parsing requires ASN.1 implementation");
    }

    pub fn verify(
        self: Certificate,
        trusted_roots: []const Certificate,
    ) !void {
        // 证书链验证
        _ = self;
        _ = trusted_roots;
    }
};
```

---

## 七、安全最佳实践

### 7.1 安全随机数

```zig
// CSPRNG (加密安全伪随机数生成器)
pub fn secureRandom() void {
    var buffer: [32]u8 = undefined;

    // 使用操作系统提供的 CSPRNG
    crypto.random.bytes(&buffer);

    // 或者使用 std.crypto.random
    const Random = @TypeOf(crypto.random);
    var rng = crypto.random;
    const value = rng.int(u64);
    _ = value;
}

// 密钥生成
pub fn generateKey(comptime length: usize) [length]u8 {
    var key: [length]u8 = undefined;
    crypto.random.bytes(&key);
    return key;
}
```

### 7.2 常量时间操作

```zig
// 防止时序攻击的常量时间比较
pub fn constantTimeEql(a: []const u8, b: []const u8) bool {
    if (a.len != b.len) return false;

    var result: u8 = 0;
    for (a, b) |x, y| {
        result |= x ^ y;
    }

    return result == 0;
}

// 使用标准库的 secureZero
pub fn clearSecret(data: []u8) void {
    crypto.utils.secureZero(u8, data);
}
```

### 7.3 内存安全

```zig
// 自动清零的秘密数据
pub fn Secret(comptime T: type) type {
    return struct {
        data: T,

        pub fn init(data: T) @This() {
            return .{ .data = data };
        }

        pub fn deinit(self: *@This()) void {
            // 清零内存
            const bytes = std.mem.asBytes(&self.data);
            crypto.utils.secureZero(u8, bytes);
        }

        pub fn get(self: @This()) T {
            return self.data;
        }
    };
}
```

---

## 八、实战：安全通信协议

### 8.1 噪声协议框架

```zig
// Noise Protocol (现代加密通信框架)
pub const NoiseProtocol = struct {
    const Pattern = enum {
        Noise_NN,   // 无静态密钥
        Noise_KN,   // 发起者有静态密钥
        Noise_NK,   // 响应者有静态密钥
        Noise_KK,   // 双方都有静态密钥
        Noise_IX,   // 发起者传输静态密钥
        Noise_XX,   // 双方传输静态密钥
    };

    state: HandshakeState,
    cipher_state: CipherState,

    pub fn init(pattern: Pattern, initiator: bool) NoiseProtocol {
        _ = pattern;
        _ = initiator;
        return .{
            .state = .{},
            .cipher_state = .{},
        };
    }

    pub fn writeMessage(self: *NoiseProtocol, payload: []const u8, out: []u8) !usize {
        // 加密并发送握手消息
        _ = self;
        _ = payload;
        _ = out;
        return 0;
    }

    pub fn readMessage(self: *NoiseProtocol, message: []const u8, out: []u8) !usize {
        // 解密握手消息
        _ = self;
        _ = message;
        _ = out;
        return 0;
    }
};

const HandshakeState = struct {};
const CipherState = struct {};
```

### 8.2 安全消息协议

```zig
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
```

---

> **文档信息**
>
> - 创建时间: 2026-03-10
> - 安全警告: 所有加密代码应经过专业审计后用于生产环境
