# 密码学原语实现 (Cryptographic Primitives)

> **难度**: L5-L6 | **预估学习时间**: 10-12小时
> **技术栈**: OpenSSL 3.x, libsodium, mbedTLS
> **安全等级**: 生产环境可用

---

## 1. 密码学安全基础

### 1.1 安全编程黄金法则

```text
密码学安全黄金法则
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

1. 不要自己实现密码学算法（除非你是密码学家）
2. 使用经过审核的库（OpenSSL, libsodium, BearSSL）
3. 密钥使用后立即清零（secure_memzero）
4. 使用加密安全随机数生成器（CSPRNG）
5. 使用时序安全函数（constant-time）
6. 正确处理填充和IV/Nonce
7. 定期更新依赖库（关注CVE）
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

### 1.2 密钥生命周期管理

```c
// secure_memory.h
#ifndef SECURE_MEMORY_H
#define SECURE_MEMORY_H

#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>

// 安全分配（锁定内存，防止交换到磁盘）
static inline void* secure_alloc(size_t size) {
    void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) return NULL;

    // 锁定物理内存
    if (mlock(ptr, size) != 0) {
        munmap(ptr, size);
        return NULL;
    }
    return ptr;
}

// 安全释放（清零后释放）
static inline void secure_free(void* ptr, size_t size) {
    if (!ptr) return;

    // 显式清零（防止编译器优化）
    volatile unsigned char* p = ptr;
    while (size--) *p++ = 0;

    munlock(ptr, size);
    munmap(ptr, size);
}

// 安全内存结构
typedef struct {
    unsigned char* data;
    size_t size;
} SecureBuffer;

SecureBuffer* secure_buffer_create(size_t size) {
    SecureBuffer* buf = calloc(1, sizeof(SecureBuffer));
    buf->data = secure_alloc(size);
    if (!buf->data) {
        free(buf);
        return NULL;
    }
    buf->size = size;
    return buf;
}

void secure_buffer_destroy(SecureBuffer* buf) {
    if (!buf) return;
    secure_free(buf->data, buf->size);
    free(buf);
}

#endif
```

---


---

## 📑 目录

- [密码学原语实现 (Cryptographic Primitives)](#密码学原语实现-cryptographic-primitives)
  - [1. 密码学安全基础](#1-密码学安全基础)
    - [1.1 安全编程黄金法则](#11-安全编程黄金法则)
    - [1.2 密钥生命周期管理](#12-密钥生命周期管理)
  - [📑 目录](#-目录)
  - [2. 哈希函数家族](#2-哈希函数家族)
    - [2.1 SHA-2系列 (SHA-256/SHA-512)](#21-sha-2系列-sha-256sha-512)
    - [2.2 HMAC（哈希消息认证码）](#22-hmac哈希消息认证码)
    - [2.3 BLAKE2（现代哈希）](#23-blake2现代哈希)
  - [3. 对称加密](#3-对称加密)
    - [3.1 AES-GCM认证加密](#31-aes-gcm认证加密)
    - [3.2 ChaCha20-Poly1305](#32-chacha20-poly1305)
  - [4. 非对称加密](#4-非对称加密)
    - [4.1 RSA（密钥生成、加密、签名）](#41-rsa密钥生成加密签名)
    - [4.2 椭圆曲线加密 (ECDSA/Ed25519)](#42-椭圆曲线加密-ecdsaed25519)
  - [5. 安全随机数](#5-安全随机数)
    - [5.1 CSPRNG（加密安全随机数生成器）](#51-csprng加密安全随机数生成器)
  - [6. 时序攻击防护](#6-时序攻击防护)
    - [6.1 常数时间操作](#61-常数时间操作)
  - [7. 实际应用：安全通信协议](#7-实际应用安全通信协议)
  - [8. 最佳实践总结](#8-最佳实践总结)
  - [9. 参考文献](#9-参考文献)


---

## 2. 哈希函数家族

### 2.1 SHA-2系列 (SHA-256/SHA-512)

```c
// openssl_sha.c
#include <openssl/evp.h>
#include <openssl/err.h>

typedef enum {
    HASH_SHA256,
    HASH_SHA384,
    HASH_SHA512,
    HASH_SHA3_256
} HashType;

static const EVP_MD* get_md(HashType type) {
    switch (type) {
        case HASH_SHA256: return EVP_sha256();
        case HASH_SHA384: return EVP_sha384();
        case HASH_SHA512: return EVP_sha512();
        case HASH_SHA3_256: return EVP_sha3_256();
        default: return NULL;
    }
}

// 一次性哈希
int hash_digest(HashType type,
                const unsigned char* data, size_t data_len,
                unsigned char* hash, size_t* hash_len) {
    const EVP_MD* md = get_md(type);
    if (!md) return -1;

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) return -1;

    int ret = -1;
    if (EVP_DigestInit_ex(ctx, md, NULL) != 1) goto cleanup;
    if (EVP_DigestUpdate(ctx, data, data_len) != 1) goto cleanup;
    if (EVP_DigestFinal_ex(ctx, hash, (unsigned int*)hash_len) != 1) goto cleanup;

    ret = 0;
cleanup:
    EVP_MD_CTX_free(ctx);
    return ret;
}

// 增量哈希（大文件）
typedef struct {
    EVP_MD_CTX* ctx;
} HashContext;

HashContext* hash_create_context(HashType type) {
    HashContext* ctx = calloc(1, sizeof(HashContext));
    ctx->ctx = EVP_MD_CTX_new();
    if (!ctx->ctx) {
        free(ctx);
        return NULL;
    }

    const EVP_MD* md = get_md(type);
    if (EVP_DigestInit_ex(ctx->ctx, md, NULL) != 1) {
        EVP_MD_CTX_free(ctx->ctx);
        free(ctx);
        return NULL;
    }
    return ctx;
}

int hash_update(HashContext* ctx, const unsigned char* data, size_t len) {
    return EVP_DigestUpdate(ctx->ctx, data, len) == 1 ? 0 : -1;
}

int hash_final(HashContext* ctx, unsigned char* hash, size_t* hash_len) {
    int ret = EVP_DigestFinal_ex(ctx->ctx, hash, (unsigned int*)hash_len);
    EVP_MD_CTX_free(ctx->ctx);
    free(ctx);
    return ret == 1 ? 0 : -1;
}

// 使用示例
void hash_file_example() {
    FILE* fp = fopen("large_file.bin", "rb");
    if (!fp) return;

    HashContext* ctx = hash_create_context(HASH_SHA256);
    if (!ctx) {
        fclose(fp);
        return;
    }

    unsigned char buffer[8192];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        hash_update(ctx, buffer, bytes_read);
    }

    unsigned char hash[32];
    size_t hash_len;
    hash_final(ctx, hash, &hash_len);

    fclose(fp);

    // 打印哈希
    for (size_t i = 0; i < hash_len; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}
```

### 2.2 HMAC（哈希消息认证码）

```c
#include <openssl/hmac.h>

// HMAC计算
int hmac_digest(HashType type,
                const unsigned char* key, size_t key_len,
                const unsigned char* data, size_t data_len,
                unsigned char* mac, size_t* mac_len) {
    const EVP_MD* md = get_md(type);
    if (!md) return -1;

    if (HMAC(md, key, key_len, data, data_len, mac, (unsigned int*)mac_len) == NULL) {
        return -1;
    }
    return 0;
}

// 密钥派生（HKDF简化版）
int hkdf_extract(HashType type,
                 const unsigned char* salt, size_t salt_len,
                 const unsigned char* ikm, size_t ikm_len,
                 unsigned char* prk, size_t* prk_len) {
    return hmac_digest(type, salt, salt_len, ikm, ikm_len, prk, prk_len);
}

int hkdf_expand(HashType type,
                const unsigned char* prk, size_t prk_len,
                const unsigned char* info, size_t info_len,
                unsigned char* okm, size_t okm_len) {
    const EVP_MD* md = get_md(type);
    size_t hash_len = EVP_MD_size(md);
    size_t n = (okm_len + hash_len - 1) / hash_len;

    if (n > 255) return -1;  // HKDF限制

    unsigned char t[EVP_MAX_MD_SIZE];
    size_t t_len = 0;

    for (size_t i = 1; i <= n; i++) {
        HMAC_CTX* ctx = HMAC_CTX_new();
        HMAC_Init_ex(ctx, prk, prk_len, md, NULL);

        if (t_len > 0) {
            HMAC_Update(ctx, t, t_len);
        }
        HMAC_Update(ctx, info, info_len);
        unsigned char counter = (unsigned char)i;
        HMAC_Update(ctx, &counter, 1);

        unsigned int len;
        HMAC_Final(ctx, t, &len);
        t_len = len;
        HMAC_CTX_free(ctx);

        size_t copy_len = (i == n && okm_len % hash_len != 0)
                         ? okm_len % hash_len : hash_len;
        memcpy(okm + (i-1) * hash_len, t, copy_len);
    }

    return 0;
}
```

### 2.3 BLAKE2（现代哈希）

```c
// 使用libsodium实现BLAKE2b
#include <sodium.h>

// 初始化
void init_crypto(void) {
    if (sodium_init() < 0) {
        fprintf(stderr, "libsodium初始化失败\n");
        exit(1);
    }
}

// BLAKE2b哈希
int blake2b_hash(const unsigned char* data, size_t data_len,
                 unsigned char hash[crypto_generichash_BYTES]) {
    return crypto_generichash(hash, crypto_generichash_BYTES,
                              data, data_len,
                              NULL, 0);  // 无密钥模式
}

// BLAKE2b带密钥（MAC）
int blake2b_mac(const unsigned char* data, size_t data_len,
                const unsigned char key[crypto_generichash_KEYBYTES],
                unsigned char hash[crypto_generichash_BYTES]) {
    return crypto_generichash(hash, crypto_generichash_BYTES,
                              data, data_len,
                              key, crypto_generichash_KEYBYTES);
}

// 密码哈希（Argon2）
int password_hash(const char* password,
                  unsigned char hash[crypto_pwhash_STRBYTES]) {
    return crypto_pwhash_str(hash, password, strlen(password),
                             crypto_pwhash_OPSLIMIT_INTERACTIVE,
                             crypto_pwhash_MEMLIMIT_INTERACTIVE);
}

int password_verify(const char* password,
                    const unsigned char hash[crypto_pwhash_STRBYTES]) {
    return crypto_pwhash_str_verify(hash, password, strlen(password));
}
```

---

## 3. 对称加密

### 3.1 AES-GCM认证加密

```c
#include <openssl/evp.h>

#define AES_GCM_KEY_SIZE 32
#define AES_GCM_IV_SIZE 12
#define AES_GCM_TAG_SIZE 16

typedef struct {
    unsigned char key[AES_GCM_KEY_SIZE];
    unsigned char iv[AES_GCM_IV_SIZE];
} AesGcmParams;

// 加密
int aes_gcm_encrypt(const AesGcmParams* params,
                    const unsigned char* plaintext, size_t pt_len,
                    const unsigned char* aad, size_t aad_len,
                    unsigned char* ciphertext,
                    unsigned char tag[AES_GCM_TAG_SIZE]) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return -1;

    int len, ret = -1;

    // 初始化加密
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1)
        goto cleanup;

    // 设置IV长度
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, AES_GCM_IV_SIZE, NULL) != 1)
        goto cleanup;

    // 设置密钥和IV
    if (EVP_EncryptInit_ex(ctx, NULL, NULL, params->key, params->iv) != 1)
        goto cleanup;

    // 处理AAD
    if (aad && aad_len > 0) {
        if (EVP_EncryptUpdate(ctx, NULL, &len, aad, aad_len) != 1)
            goto cleanup;
    }

    // 加密
    if (EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, pt_len) != 1)
        goto cleanup;
    int ciphertext_len = len;

    // 最终化
    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1)
        goto cleanup;
    ciphertext_len += len;

    // 获取认证标签
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, AES_GCM_TAG_SIZE, tag) != 1)
        goto cleanup;

    ret = ciphertext_len;

cleanup:
    EVP_CIPHER_CTX_free(ctx);
    return ret;
}

// 解密
int aes_gcm_decrypt(const AesGcmParams* params,
                    const unsigned char* ciphertext, size_t ct_len,
                    const unsigned char* aad, size_t aad_len,
                    const unsigned char tag[AES_GCM_TAG_SIZE],
                    unsigned char* plaintext) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return -1;

    int len, ret = -1;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1)
        goto cleanup;

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, AES_GCM_IV_SIZE, NULL) != 1)
        goto cleanup;

    if (EVP_DecryptInit_ex(ctx, NULL, NULL, params->key, params->iv) != 1)
        goto cleanup;

    if (aad && aad_len > 0) {
        if (EVP_DecryptUpdate(ctx, NULL, &len, aad, aad_len) != 1)
            goto cleanup;
    }

    if (EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ct_len) != 1)
        goto cleanup;
    int plaintext_len = len;

    // 设置期望的标签值
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, AES_GCM_TAG_SIZE, (void*)tag) != 1)
        goto cleanup;

    // 验证标签（解密Final会进行验证）
    if (EVP_DecryptFinal_ex(ctx, plaintext + len, &len) != 1) {
        // 验证失败
        goto cleanup;
    }
    plaintext_len += len;
    ret = plaintext_len;

cleanup:
    EVP_CIPHER_CTX_free(ctx);
    return ret;  // 返回-1表示验证失败
}
```

### 3.2 ChaCha20-Poly1305

```c
// libsodium实现
#include <sodium.h>

#define CHACHA20_KEY_SIZE crypto_aead_chacha20poly1305_IETF_KEYBYTES
#define CHACHA20_NONCE_SIZE crypto_aead_chacha20poly1305_IETF_NPUBBYTES
#define CHACHA20_TAG_SIZE crypto_aead_chacha20poly1305_IETF_ABYTES

int chacha20_encrypt(const unsigned char key[CHACHA20_KEY_SIZE],
                     const unsigned char nonce[CHACHA20_NONCE_SIZE],
                     const unsigned char* plaintext, size_t pt_len,
                     const unsigned char* aad, size_t aad_len,
                     unsigned char* ciphertext,
                     unsigned char tag[CHACHA20_TAG_SIZE]) {
    unsigned long long ciphertext_len;
    int ret = crypto_aead_chacha20poly1305_ietf_encrypt(
        ciphertext, &ciphertext_len,
        plaintext, pt_len,
        aad, aad_len,
        NULL,  // nsec（未使用）
        nonce, key);

    if (ret == 0) {
        // 分离标签（最后16字节）
        memcpy(tag, ciphertext + pt_len, CHACHA20_TAG_SIZE);
    }

    return ret;
}

// ChaCha20优于AES的场景：
// 1. 无AES-NI硬件的旧CPU
// 2. 移动设备（ARM）
// 3. 需要恒定时间实现
```

---

## 4. 非对称加密

### 4.1 RSA（密钥生成、加密、签名）

```c
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

// 生成RSA密钥对
int rsa_generate_keypair(int bits, const char* priv_file, const char* pub_file) {
    BIGNUM* bne = BN_new();
    if (!BN_set_word(bne, RSA_F4)) {  // 65537
        BN_free(bne);
        return -1;
    }

    RSA* rsa = RSA_new();
    if (!RSA_generate_key_ex(rsa, bits, bne, NULL)) {
        RSA_free(rsa);
        BN_free(bne);
        return -1;
    }
    BN_free(bne);

    // 保存私钥
    FILE* fp = fopen(priv_file, "wb");
    if (fp) {
        PEM_write_RSAPrivateKey(fp, rsa, NULL, NULL, 0, NULL, NULL);
        fclose(fp);
    }

    // 保存公钥
    fp = fopen(pub_file, "wb");
    if (fp) {
        PEM_write_RSA_PUBKEY(fp, rsa);
        fclose(fp);
    }

    RSA_free(rsa);
    return 0;
}

// RSA-OAEP加密（推荐）
int rsa_encrypt_oaep(const unsigned char* plaintext, size_t pt_len,
                     unsigned char* ciphertext, RSA* pub_key) {
    return RSA_public_encrypt(pt_len, plaintext, ciphertext,
                              pub_key, RSA_PKCS1_OAEP_PADDING);
}

// RSA-PSS签名
int rsa_sign_pss(const unsigned char* msg, size_t msg_len,
                 unsigned char* sig, size_t* sig_len,
                 RSA* priv_key, HashType hash_type) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_PKEY* pkey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(pkey, priv_key);

    EVP_DigestSignInit(ctx, NULL, get_md(hash_type), NULL, pkey);
    EVP_DigestSignUpdate(ctx, msg, msg_len);
    EVP_DigestSignFinal(ctx, sig, sig_len);

    EVP_PKEY_free(pkey);
    EVP_MD_CTX_free(ctx);
    return 0;
}
```

### 4.2 椭圆曲线加密 (ECDSA/Ed25519)

```c
// ECDSA (secp256k1 - 比特币使用)
#include <openssl/ec.h>

int ecdsa_sign(const unsigned char* hash, size_t hash_len,
               unsigned char* sig, size_t* sig_len,
               EC_KEY* ec_key) {
    ECDSA_SIG* signature = ECDSA_do_sign(hash, hash_len, ec_key);
    if (!signature) return -1;

    // 序列化为DER
    unsigned char* sig_ptr = sig;
    *sig_len = i2d_ECDSA_SIG(signature, &sig_ptr);

    ECDSA_SIG_free(signature);
    return 0;
}

int ecdsa_verify(const unsigned char* hash, size_t hash_len,
                 const unsigned char* sig, size_t sig_len,
                 EC_KEY* ec_key) {
    const unsigned char* p = sig;
    ECDSA_SIG* signature = d2i_ECDSA_SIG(NULL, &p, sig_len);
    if (!signature) return -1;

    int ret = ECDSA_do_verify(hash, hash_len, signature, ec_key);
    ECDSA_SIG_free(signature);
    return ret;  // 1=valid, 0=invalid, -1=error
}

// Ed25519（libsodium - 推荐用于新设计）
void ed25519_example(void) {
    unsigned char pk[crypto_sign_PUBLICKEYBYTES];
    unsigned char sk[crypto_sign_SECRETKEYBYTES];

    // 生成密钥对
    crypto_sign_keypair(pk, sk);

    // 签名
    const char* message = "Hello, World!";
    unsigned char signed_msg[crypto_sign_BYTES + 13];
    unsigned long long signed_len;
    crypto_sign(signed_msg, &signed_len,
                (const unsigned char*)message, strlen(message),
                sk);

    // 验证
    unsigned char verified_msg[13];
    unsigned long long verified_len;
    int ret = crypto_sign_open(verified_msg, &verified_len,
                               signed_msg, signed_len,
                               pk);
    if (ret == 0) {
        printf("签名验证成功\n");
    }
}
```

---

## 5. 安全随机数

### 5.1 CSPRNG（加密安全随机数生成器）

```c
// Linux getrandom (内核3.17+)
#include <sys/random.h>

int secure_random(void* buf, size_t len) {
    size_t total = 0;
    while (total < len) {
        ssize_t ret = getrandom((char*)buf + total, len - total, 0);
        if (ret < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        total += ret;
    }
    return 0;
}

// OpenSSL CSPRNG
int openssl_random(void* buf, size_t len) {
    return RAND_bytes(buf, len) == 1 ? 0 : -1;
}

// libsodium随机
void sodium_random_example(void) {
    // 生成随机32字节
    unsigned char random_bytes[32];
    randombytes_buf(random_bytes, sizeof(random_bytes));

    // 生成随机32位整数
    uint32_t random_int = randombytes_random();

    // 生成范围内的随机数
    uint32_t bounded = randombytes_uniform(100);  // 0-99
}
```

---

## 6. 时序攻击防护

### 6.1 常数时间操作

```c
// 常数时间内存比较（防时序攻击）
int constant_time_compare(const volatile unsigned char* a,
                          const volatile unsigned char* b,
                          size_t len) {
    volatile unsigned char result = 0;
    for (size_t i = 0; i < len; i++) {
        result |= a[i] ^ b[i];
    }
    return result;  // 0 if equal, non-zero otherwise
}

// 常数时间选择（避免分支）
unsigned char constant_time_select(unsigned char condition,
                                   unsigned char a,
                                   unsigned char b) {
    // condition: 0xff (true) or 0x00 (false)
    return (condition & a) | ((~condition) & b);
}

// 常数时间整数比较
int constant_time_lt(uint64_t a, uint64_t b) {
    // 返回0xff如果a < b，否则0x00
    return (unsigned char)((a - b) >> 63) * 0xff;
}

// 使用sodium的常数时间函数
#include <sodium/utils.h>
void sodium_constant_time_example(void) {
    unsigned char a[32], b[32];
    // ... 填充数据

    // 常数时间比较
    int eq = sodium_memcmp(a, b, 32);

    // 常数时间是否为零检查
    int is_zero = sodium_is_zero(a, 32);

    // 安全清零
    sodium_memzero(a, 32);
}
```

---

## 7. 实际应用：安全通信协议

```c
// 简化的安全客户端
typedef struct {
    SSL_CTX* ctx;
    SSL* ssl;
    int socket;
} SecureConnection;

SecureConnection* secure_connect(const char* host, int port) {
    SecureConnection* conn = calloc(1, sizeof(SecureConnection));

    // 初始化SSL
    SSL_library_init();
    conn->ctx = SSL_CTX_new(TLS_client_method());

    // 强制TLS 1.3
    SSL_CTX_set_min_proto_version(conn->ctx, TLS1_3_VERSION);

    // 设置证书验证
    SSL_CTX_set_default_verify_paths(conn->ctx);
    SSL_CTX_set_verify(conn->ctx, SSL_VERIFY_PEER, NULL);

    // 创建socket
    conn->socket = socket(AF_INET, SOCK_STREAM, 0);
    // ... 连接代码

    // 创建SSL连接
    conn->ssl = SSL_new(conn->ctx);
    SSL_set_fd(conn->ssl, conn->socket);
    SSL_set_tlsext_host_name(conn->ssl, host);  // SNI

    if (SSL_connect(conn->ssl) <= 0) {
        // 连接失败
        secure_close(conn);
        return NULL;
    }

    // 验证证书
    X509* cert = SSL_get_peer_certificate(conn->ssl);
    if (cert) {
        long verify = SSL_get_verify_result(conn->ssl);
        if (verify != X509_V_OK) {
            fprintf(stderr, "证书验证失败\n");
        }
        X509_free(cert);
    }

    return conn;
}

int secure_send(SecureConnection* conn, const void* data, size_t len) {
    return SSL_write(conn->ssl, data, len);
}

int secure_recv(SecureConnection* conn, void* buf, size_t len) {
    return SSL_read(conn->ssl, buf, len);
}
```

---

## 8. 最佳实践总结

| 场景 | 推荐算法 | 避免使用 |
|:-----|:---------|:---------|
| 哈希 | SHA-256/SHA-3 | MD5, SHA-1 |
| 密码哈希 | Argon2, bcrypt | SHA-256+salt, PBKDF2（新设计） |
| 对称加密 | AES-256-GCM, ChaCha20-Poly1305 | AES-CBC无认证, RC4 |
| 非对称签名 | Ed25519, ECDSA P-256 | RSA PKCS#1 v1.5 |
| 密钥交换 | X25519 (ECDH) | RSA密钥交换 |
| 随机数 | /dev/urandom, getrandom | rand(), random() |

---

## 9. 参考文献

1. **Ferguson, N.** et al. (2015). *Cryptography Engineering*. Wiley.
2. **OpenSSL Documentation**: <https://www.openssl.org/docs/>
3. **libsodium Documentation**: <https://doc.libsodium.org/>
4. **OWASP Cryptographic Storage Cheat Sheet**

---

> **关联文档**: [区块链基础](./02_Blockchain_Basics.md) | [硬件安全](../../03_System_Technology_Domains/07_Hardware_Security/README.md) | [Zig安全](../../Zig/Zig_Security_Cryptography.md)
