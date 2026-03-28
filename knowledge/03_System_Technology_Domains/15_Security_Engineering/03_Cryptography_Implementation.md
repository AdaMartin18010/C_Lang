# 密码学实现安全

> **层级**: L4 (方法论层)
> **目标**: 掌握现代密码学库的安全使用、密钥生命周期管理与攻防实践
> **更新日期**: 2026-03-28
> **适用版本**: OpenSSL 3.x, libsodium 1.0.19+, mbedTLS 3.x

---

## 目录

- [密码学实现安全](#密码学实现安全)
  - [目录](#目录)
  - [本节概要](#本节概要)
  - [密码学基础回顾](#密码学基础回顾)
    - [现代密码学原则](#现代密码学原则)
    - [算法分类速查](#算法分类速查)
  - [对称加密实现](#对称加密实现)
    - [AES-GCM 实现详解](#aes-gcm-实现详解)
    - [ChaCha20-Poly1305 实现](#chacha20-poly1305-实现)
  - [非对称加密实现](#非对称加密实现)
    - [X25519 密钥交换](#x25519-密钥交换)
    - [Ed25519 数字签名](#ed25519-数字签名)
  - [哈希与消息认证](#哈希与消息认证)
    - [SHA-3 和 SHAKE 实现](#sha-3-和-shake-实现)
  - [密钥派生与密钥管理](#密钥派生与密钥管理)
    - [Argon2id 密码哈希](#argon2id-密码哈希)
    - [HKDF 密钥派生](#hkdf-密钥派生)
  - [随机数生成安全](#随机数生成安全)
    - [加密安全随机数](#加密安全随机数)
  - [常见攻击与防护](#常见攻击与防护)
    - [时序攻击防护](#时序攻击防护)
    - [填充Oracle防护](#填充oracle防护)
  - [密码学库对比与选择](#密码学库对比与选择)
  - [实践指南](#实践指南)
    - [完整加密通信示例](#完整加密通信示例)
  - [层次关联与映射分析](#层次关联与映射分析)
  - [决策矩阵](#决策矩阵)
    - [算法选择决策树](#算法选择决策树)
    - [密码学库选择矩阵](#密码学库选择矩阵)
  - [相关资源](#相关资源)
    - [官方文档](#官方文档)
    - [标准规范](#标准规范)
    - [学习资源](#学习资源)
    - [工具](#工具)
  - [质量验收清单](#质量验收清单)

---

## 本节概要

| 属性 | 内容 |
|------|------|
| **核心主题** | 现代密码学算法的安全实现与最佳实践 |
| **关键算法** | AES-GCM, ChaCha20-Poly1305, X25519, Ed25519, SHA-3, Argon2 |
| **主流库** | OpenSSL 3.x, libsodium, mbedTLS, WolfSSL, AWS-LC |
| **安全目标** | 机密性、完整性、认证性、不可否认性 |
| **常见威胁** | 时序攻击、填充Oracle、密钥泄露、随机数预测 |
| **适用场景** | 数据传输加密、存储加密、身份认证、数字签名 |

---

## 密码学基础回顾

### 现代密码学原则

现代密码学实现遵循以下核心原则：

```
┌─────────────────────────────────────────────────────────────────┐
│                    现代密码学安全原则                           │
├─────────────────────────────────────────────────────────────────┤
│  1. Kerckhoffs原理: 安全性应仅依赖于密钥，而非算法保密         │
│  2. 最小权限原则: 仅使用必要强度的算法和密钥长度               │
│  3. 前向保密: 即使长期密钥泄露，历史会话仍安全                 │
│  4. 抗量子准备: 考虑后量子密码学算法的迁移路径                 │
│  5. 纵深防御: 多层安全机制，不依赖单一防护点                   │
└─────────────────────────────────────────────────────────────────┘
```

### 算法分类速查

| 类别 | 算法示例 | 用途 | 推荐状态 |
|------|----------|------|----------|
| **对称加密** | AES-256-GCM, ChaCha20-Poly1305 | 大量数据加密 | ✅ 推荐 |
| **非对称加密** | RSA-OAEP, ECIES (X25519) | 密钥交换、小数据加密 | ✅ 推荐 |
| **数字签名** | Ed25519, ECDSA (P-256) | 身份认证、不可否认 | ✅ 推荐 |
| **哈希函数** | SHA-256, SHA-3-256, BLAKE3 | 数据完整性、指纹 | ✅ 推荐 |
| **MAC/认证** | HMAC-SHA256, KMAC256 | 消息认证 | ✅ 推荐 |
| **密钥派生** | Argon2id, PBKDF2, HKDF | 密码哈希、密钥派生 | ✅ 推荐 |
| **废弃算法** | MD5, SHA-1, DES, 3DES, RSA-PKCS1-v1.5 | - | ❌ 禁用 |

---

## 对称加密实现

### AES-GCM 实现详解

AES-GCM (Galois/Counter Mode) 是目前最广泛使用的认证加密模式，提供机密性和完整性保护。

```c
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string.h>

#define AES_GCM_KEY_SIZE 32
#define AES_GCM_IV_SIZE 12
#define AES_GCM_TAG_SIZE 16

/**
 * AES-256-GCM 加密实现
 *
 * 安全要点:
 * 1. IV必须每次随机生成，永不重复
 * 2. 同一密钥下IV重复会导致完全安全崩溃
 * 3. 认证标签必须验证，否则易遭受填充Oracle攻击
 */
int aes_gcm_encrypt(
    const unsigned char *plaintext, int plaintext_len,
    const unsigned char *aad, int aad_len,
    const unsigned char *key,
    unsigned char *iv,                  // 输出: 12字节随机IV
    unsigned char *ciphertext,          // 输出: 密文
    unsigned char *tag                  // 输出: 16字节认证标签
) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return -1;

    // 生成随机IV (12字节是GCM推荐值)
    if (RAND_bytes(iv, AES_GCM_IV_SIZE) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    // 初始化加密操作
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1)
        goto err;

    // 设置IV长度
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, AES_GCM_IV_SIZE, NULL) != 1)
        goto err;

    // 初始化密钥和IV
    if (EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv) != 1)
        goto err;

    int len;
    // 加密附加认证数据(AAD)
    if (aad && aad_len > 0) {
        if (EVP_EncryptUpdate(ctx, NULL, &len, aad, aad_len) != 1)
            goto err;
    }

    // 加密明文
    if (EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len) != 1)
        goto err;
    int ciphertext_len = len;

    // 完成加密
    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1)
        goto err;
    ciphertext_len += len;

    // 获取认证标签
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, AES_GCM_TAG_SIZE, tag) != 1)
        goto err;

    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;

err:
    EVP_CIPHER_CTX_free(ctx);
    return -1;
}

/**
 * AES-256-GCM 解密实现
 *
 * 安全要点:
 * 1. 必须在获取明文前验证认证标签
 * 2. 验证失败必须立即终止，不返回任何明文数据
 * 3. 实现必须常量时间，防止时序攻击
 */
int aes_gcm_decrypt(
    const unsigned char *ciphertext, int ciphertext_len,
    const unsigned char *aad, int aad_len,
    const unsigned char *tag,
    const unsigned char *key,
    const unsigned char *iv,
    unsigned char *plaintext
) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return -1;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1)
        goto err;

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, AES_GCM_IV_SIZE, NULL) != 1)
        goto err;

    if (EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv) != 1)
        goto err;

    int len;
    // 处理AAD
    if (aad && aad_len > 0) {
        if (EVP_DecryptUpdate(ctx, NULL, &len, aad, aad_len) != 1)
            goto err;
    }

    // 解密密文
    if (EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len) != 1)
        goto err;
    int plaintext_len = len;

    // 设置预期的认证标签
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, AES_GCM_TAG_SIZE, (void *)tag) != 1)
        goto err;

    // 验证标签并完成解密
    // 返回1表示验证成功，0或负值表示失败
    int ret = EVP_DecryptFinal_ex(ctx, plaintext + len, &len);

    EVP_CIPHER_CTX_free(ctx);

    if (ret > 0) {
        plaintext_len += len;
        return plaintext_len;
    }
    // 认证失败 - 不返回任何数据
    return -1;

err:
    EVP_CIPHER_CTX_free(ctx);
    return -1;
}
```

### ChaCha20-Poly1305 实现

ChaCha20-Poly1305 是现代替代 AES-GCM 的选择，在纯软件实现中更快，且对nonce误用更鲁棒。

```c
#include <openssl/evp.h>

#define CHACHA20_KEY_SIZE 32
#define CHACHA20_NONCE_SIZE 12
#define POLY1305_TAG_SIZE 16

/**
 * ChaCha20-Poly1305 加密
 *
 * 优势:
 * - 软件实现性能优于AES (无AES-NI时)
 * - 对时序攻击天然免疫 (无查表)
 * - 移动端和嵌入式设备更友好
 */
int chacha20_poly1305_encrypt(
    const unsigned char *plaintext, int plaintext_len,
    const unsigned char *aad, int aad_len,
    const unsigned char *key,
    unsigned char *nonce,
    unsigned char *ciphertext,
    unsigned char *tag
) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return -1;

    // 生成随机nonce
    if (RAND_bytes(nonce, CHACHA20_NONCE_SIZE) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if (EVP_EncryptInit_ex(ctx, EVP_chacha20_poly1305(), NULL, NULL, NULL) != 1)
        goto err;

    // ChaCha20-Poly1305使用固定长度的nonce
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, CHACHA20_NONCE_SIZE, NULL) != 1)
        goto err;

    if (EVP_EncryptInit_ex(ctx, NULL, NULL, key, nonce) != 1)
        goto err;

    int len;
    if (aad && aad_len > 0) {
        if (EVP_EncryptUpdate(ctx, NULL, &len, aad, aad_len) != 1)
            goto err;
    }

    if (EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len) != 1)
        goto err;
    int ciphertext_len = len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1)
        goto err;
    ciphertext_len += len;

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, POLY1305_TAG_SIZE, tag) != 1)
        goto err;

    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;

err:
    EVP_CIPHER_CTX_free(ctx);
    return -1;
}
```

---

## 非对称加密实现

### X25519 密钥交换

X25519 是现代密钥交换的首选算法，基于Curve25519，提供128位安全性。

```c
#include <openssl/evp.h>
#include <openssl/pem.h>

#define X25519_KEY_SIZE 32

/**
 * X25519密钥对生成
 *
 * X25519优势:
 * - 恒定时间实现，天然免疫时序攻击
 * - 小密钥尺寸 (32字节)
 * - 快速密钥生成
 * - 无需要求验证点 ( Twist安全 )
 */
int x25519_generate_keypair(unsigned char *public_key, unsigned char *private_key) {
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_X25519, NULL);
    if (!ctx) return -1;

    EVP_PKEY *pkey = NULL;
    if (EVP_PKEY_keygen_init(ctx) <= 0)
        goto err;

    if (EVP_PKEY_keygen(ctx, &pkey) <= 0)
        goto err;

    // 提取私钥
    size_t priv_len = X25519_KEY_SIZE;
    if (EVP_PKEY_get_raw_private_key(pkey, private_key, &priv_len) != 1)
        goto err;

    // 提取公钥
    size_t pub_len = X25519_KEY_SIZE;
    if (EVP_PKEY_get_raw_public_key(pkey, public_key, &pub_len) != 1)
        goto err;

    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    return 0;

err:
    if (pkey) EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    return -1;
}

/**
 * X25519密钥协商
 *
 * 输出: 32字节共享密钥 (需使用KDF派生实际密钥)
 */
int x25519_derive_shared_secret(
    const unsigned char *private_key,
    const unsigned char *peer_public_key,
    unsigned char *shared_secret
) {
    EVP_PKEY *priv = EVP_PKEY_new_raw_private_key(
        EVP_PKEY_X25519, NULL, private_key, X25519_KEY_SIZE);
    if (!priv) return -1;

    EVP_PKEY *pub = EVP_PKEY_new_raw_public_key(
        EVP_PKEY_X25519, NULL, peer_public_key, X25519_KEY_SIZE);
    if (!pub) {
        EVP_PKEY_free(priv);
        return -1;
    }

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(priv, NULL);
    if (!ctx) goto err;

    if (EVP_PKEY_derive_init(ctx) <= 0)
        goto err;

    if (EVP_PKEY_derive_set_peer(ctx, pub) <= 0)
        goto err;

    size_t secret_len = X25519_KEY_SIZE;
    if (EVP_PKEY_derive(ctx, shared_secret, &secret_len) <= 0)
        goto err;

    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pub);
    EVP_PKEY_free(priv);
    return 0;

err:
    if (ctx) EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pub);
    EVP_PKEY_free(priv);
    return -1;
}
```

### Ed25519 数字签名

Ed25519 提供高性能、高安全性的数字签名，是现代系统的首选签名方案。

```c
#include <openssl/evp.h>

#define ED25519_SEED_SIZE 32
#define ED25519_SIG_SIZE 64

/**
 * Ed25519签名生成
 *
 * 特性:
 * - 确定性签名 (无随机数，避免熵源问题)
 * - 快速验证
 * - 批量验证优化
 * - 小签名尺寸 (64字节)
 */
int ed25519_sign(
    const unsigned char *message, size_t message_len,
    const unsigned char *private_key,
    unsigned char *signature
) {
    EVP_PKEY *pkey = EVP_PKEY_new_raw_private_key(
        EVP_PKEY_ED25519, NULL, private_key, ED25519_SEED_SIZE);
    if (!pkey) return -1;

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        EVP_PKEY_free(pkey);
        return -1;
    }

    if (EVP_DigestSignInit(mdctx, NULL, NULL, NULL, pkey) != 1)
        goto err;

    size_t sig_len = ED25519_SIG_SIZE;
    if (EVP_DigestSign(mdctx, signature, &sig_len, message, message_len) != 1)
        goto err;

    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    return 0;

err:
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    return -1;
}

/**
 * Ed25519签名验证
 */
int ed25519_verify(
    const unsigned char *message, size_t message_len,
    const unsigned char *signature, size_t sig_len,
    const unsigned char *public_key
) {
    EVP_PKEY *pkey = EVP_PKEY_new_raw_public_key(
        EVP_PKEY_ED25519, NULL, public_key, ED25519_SEED_SIZE);
    if (!pkey) return -1;

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        EVP_PKEY_free(pkey);
        return -1;
    }

    if (EVP_DigestVerifyInit(mdctx, NULL, NULL, NULL, pkey) != 1)
        goto err;

    int ret = EVP_DigestVerify(mdctx, signature, sig_len, message, message_len);

    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    return (ret == 1) ? 0 : -1;

err:
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    return -1;
}
```

---

## 哈希与消息认证

### SHA-3 和 SHAKE 实现

```c
#include <openssl/evp.h>

/**
 * SHA3-256哈希
 */
void sha3_256_hash(const unsigned char *data, size_t len, unsigned char *hash) {
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, EVP_sha3_256(), NULL);
    EVP_DigestUpdate(mdctx, data, len);
    unsigned int hash_len;
    EVP_DigestFinal_ex(mdctx, hash, &hash_len);
    EVP_MD_CTX_free(mdctx);
}

/**
 * SHAKE256可扩展输出函数(XOF)
 *
 * 用途: 需要任意长度输出的场景，如派生长密钥
 */
void shake256_xof(const unsigned char *data, size_t len,
                  unsigned char *output, size_t output_len) {
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, EVP_shake256(), NULL);
    EVP_DigestUpdate(mdctx, data, len);
    EVP_DigestFinalXOF(mdctx, output, output_len);
    EVP_MD_CTX_free(mdctx);
}

/**
 * HMAC-SHA256消息认证码
 */
void hmac_sha256(const unsigned char *key, size_t key_len,
                 const unsigned char *data, size_t data_len,
                 unsigned char *mac, unsigned int *mac_len) {
    HMAC(EVP_sha256(), key, key_len, data, data_len, mac, mac_len);
}
```

---

## 密钥派生与密钥管理

### Argon2id 密码哈希

Argon2id 是密码哈希竞赛获胜者，抵抗GPU和ASIC攻击。

```c
#include <argon2.h>

#define SALT_SIZE 16
#define HASH_SIZE 32

/**
 * Argon2id密码哈希
 *
 * 参数调优建议:
 * - m_cost (内存): 64MB-1GB，根据服务器能力
 * - t_cost (迭代): 3-10次
 * - parallelism: 4-8线程
 *
 * 推荐配置 (OWASP 2023):
 * - m=19456 (19MB), t=2, p=1 (最低交互)
 * - m=65536 (64MB), t=3, p=4 (中等安全)
 * - m=4194304 (4GB), t=1, p=4 (高安全)
 */
int password_hash_argon2id(
    const char *password,
    unsigned char *salt,           // 16字节随机盐
    unsigned char *hash,           // 32字节输出
    uint32_t m_cost,               // KB
    uint32_t t_cost,               // 迭代次数
    uint32_t parallelism
) {
    // 生成随机盐
    if (RAND_bytes(salt, SALT_SIZE) != 1)
        return -1;

    int result = argon2id_hash_raw(
        t_cost,           // 迭代次数
        m_cost,           // 内存成本 (KB)
        parallelism,      // 并行度
        password, strlen(password),
        salt, SALT_SIZE,
        hash, HASH_SIZE
    );

    return (result == ARGON2_OK) ? 0 : -1;
}

/**
 * Argon2id密码验证
 */
int password_verify_argon2id(
    const char *password,
    const unsigned char *salt,
    const unsigned char *expected_hash,
    uint32_t m_cost,
    uint32_t t_cost,
    uint32_t parallelism
) {
    unsigned char computed_hash[HASH_SIZE];

    int result = argon2id_hash_raw(
        t_cost, m_cost, parallelism,
        password, strlen(password),
        salt, SALT_SIZE,
        computed_hash, HASH_SIZE
    );

    if (result != ARGON2_OK)
        return -1;

    // 常量时间比较，防止时序攻击
    return CRYPTO_memcmp(computed_hash, expected_hash, HASH_SIZE) == 0 ? 0 : -1;
}
```

### HKDF 密钥派生

```c
#include <openssl/kdf.h>
#include <openssl/core_names.h>

/**
 * HKDF-SHA256密钥派生
 *
 * extract: 从输入密钥材料(IKM)中提取伪随机密钥(PRK)
 * expand: 从PRK派生任意长度的输出密钥材料(OKM)
 */
int hkdf_derive(
    const unsigned char *ikm, size_t ikm_len,      // 输入密钥材料
    const unsigned char *salt, size_t salt_len,    // 可选盐值
    const unsigned char *info, size_t info_len,    // 上下文信息
    unsigned char *okm, size_t okm_len             // 输出密钥材料
) {
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);
    if (!ctx) return -1;

    if (EVP_PKEY_derive_init(ctx) <= 0)
        goto err;

    // 设置HKDF模式
    if (EVP_PKEY_CTX_set_hkdf_mode(ctx, EVP_PKEY_HKDEF_MODE_EXTRACT_AND_EXPAND) <= 0)
        goto err;

    // 设置哈希算法
    if (EVP_PKEY_CTX_set_hkdf_md(ctx, EVP_sha256()) <= 0)
        goto err;

    // 设置输入密钥材料
    if (EVP_PKEY_CTX_set1_hkdf_key(ctx, ikm, ikm_len) <= 0)
        goto err;

    // 设置盐值 (可选但推荐)
    if (salt && salt_len > 0) {
        if (EVP_PKEY_CTX_set1_hkdf_salt(ctx, salt, salt_len) <= 0)
            goto err;
    }

    // 设置信息字符串
    if (info && info_len > 0) {
        if (EVP_PKEY_CTX_add1_hkdf_info(ctx, info, info_len) <= 0)
            goto err;
    }

    // 派生密钥
    if (EVP_PKEY_derive(ctx, okm, &okm_len) <= 0)
        goto err;

    EVP_PKEY_CTX_free(ctx);
    return 0;

err:
    EVP_PKEY_CTX_free(ctx);
    return -1;
}
```

---

## 随机数生成安全

### 加密安全随机数

```c
#include <openssl/rand.h>

/**
 * 生成加密安全随机字节
 *
 * OpenSSL 3.x使用:
 * - Linux: getrandom() 系统调用
 * - Windows: BCryptGenRandom
 * - 其他: /dev/urandom 或熵池
 */
int generate_secure_random(unsigned char *buffer, size_t len) {
    // RAND_bytes是线程安全的
    // 在OpenSSL 3.x中，优先使用硬件RNG (RDSEED/RDRAND)
    if (RAND_bytes(buffer, len) != 1) {
        // 失败时清除缓冲区，避免泄露状态
        memset(buffer, 0, len);
        return -1;
    }
    return 0;
}

/**
 * 随机数生成器健康检查
 */
int check_rng_health(void) {
    // 检查RNG是否有足够的熵
    if (RAND_status() != 1) {
        // RNG未初始化或熵不足
        // 在生产环境中应该阻塞或报错
        return -1;
    }
    return 0;
}
```

---

## 常见攻击与防护

### 时序攻击防护

```c
#include <openssl/crypto.h>

/**
 * 常量时间比较
 *
 * 防止时序攻击: 比较时间应与相等字节数无关
 */
int constant_time_compare(const unsigned char *a,
                          const unsigned char *b,
                          size_t len) {
    // 使用OpenSSL提供的常量时间比较
    return CRYPTO_memcmp(a, b, len) == 0 ? 0 : -1;
}

/**
 * 常量时间选择
 *
 * 条件: mask为全1时返回a，全0时返回b
 * 用途: 避免基于分支的时序泄露
 */
static inline unsigned char constant_time_select(
    unsigned char mask,
    unsigned char a,
    unsigned char b
) {
    return (mask & a) | (~mask & b);
}
```

### 填充Oracle防护

```c
/**
 * 安全解密模式 - 防止填充Oracle攻击
 *
 * 关键原则:
 * 1. 认证失败时返回通用错误，不区分具体失败原因
 * 2. 认证验证完成前不解密任何数据
 * 3. 使用认证加密(AEAD)算法，如AES-GCM
 */

// 错误码枚举 - 对外只返回成功/失败
typedef enum {
    DECRYPT_SUCCESS = 0,
    DECRYPT_FAILURE = -1
    // 不要暴露具体失败原因!
} decrypt_result_t;

decrypt_result_t secure_decrypt(
    const unsigned char *ciphertext,
    const unsigned char *mac,
    const unsigned char *key,
    unsigned char *plaintext
) {
    // 1. 首先完整验证MAC
    if (!verify_mac(ciphertext, mac, key)) {
        // 日志记录详细错误 (内部使用)
        log_security_event("MAC verification failed");

        // 对外只返回通用失败
        return DECRYPT_FAILURE;
    }

    // 2. 验证通过后才解密密文
    decrypt_ciphertext(ciphertext, key, plaintext);

    return DECRYPT_SUCCESS;
}
```

---

## 密码学库对比与选择

| 特性 | OpenSSL 3.x | libsodium | mbedTLS | WolfSSL | AWS-LC |
|------|-------------|-----------|---------|---------|--------|
| **许可证** | Apache 2.0 | ISC | Apache 2.0/GPL | GPL/Commercial | Apache 2.0 |
| **代码大小** | 大 (~2MB) | 小 (~300KB) | 中 (~500KB) | 小 (~100KB) | 中 (~1MB) |
| **FIPS 140-2** | ✅ 支持 | ❌ | ✅ 支持 | ✅ 支持 | ✅ 支持 |
| **后量子密码** | 部分 | ✅ 完整 | 部分 | 部分 | 部分 |
| **易用性** | 中 | ✅ 高 | 中 | 中 | 高 |
| **性能** | 高 | 高 | 中 | 高 | 高 |
| **嵌入式支持** | 一般 | 好 | ✅ 优秀 | ✅ 优秀 | 一般 |
| **推荐场景** | 企业/传统系统 | 新项目/安全优先 | IoT/嵌入式 | 嵌入式/实时 | AWS生态 |

---

## 实践指南

### 完整加密通信示例

```c
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

/**
 * 安全通信协议实现
 *
 * 1. X25519密钥交换建立共享密钥
 * 2. HKDF派生会话密钥
 * 3. ChaCha20-Poly1305加密消息
 * 4. Ed25519签名认证
 */

typedef struct {
    unsigned char session_key[32];
    unsigned char send_nonce[12];
    unsigned char recv_nonce[12];
    uint64_t send_counter;
    uint64_t recv_counter;
} secure_session_t;

// 初始化安全会话
int session_init(secure_session_t *session,
                 const unsigned char *shared_secret,
                 const unsigned char *salt) {
    // 派生会话密钥
    const char *info = "secure-session-v1";
    if (hkdf_derive(shared_secret, 32, salt, 32,
                    (const unsigned char *)info, strlen(info),
                    session->session_key, 32) != 0) {
        return -1;
    }

    memset(session->send_nonce, 0, 12);
    memset(session->recv_nonce, 0, 12);
    session->send_counter = 0;
    session->recv_counter = 0;

    return 0;
}

// 加密发送消息
int session_encrypt(secure_session_t *session,
                    const unsigned char *plaintext, size_t pt_len,
                    unsigned char *ciphertext, size_t *ct_len) {
    // 增加计数器并更新nonce
    session->send_counter++;
    // nonce前4字节为固定，后8字节为计数器 (小端)
    for (int i = 0; i < 8; i++) {
        session->send_nonce[4 + i] = (session->send_counter >> (i * 8)) & 0xFF;
    }

    unsigned char tag[16];
    int len = chacha20_poly1305_encrypt(
        plaintext, pt_len,
        NULL, 0,                    // 无AAD
        session->session_key,
        session->send_nonce,
        ciphertext + 16,            // 密文从偏移16开始
        tag
    );

    if (len < 0) return -1;

    // 输出格式: [12字节nonce][4字节保留][密文][16字节tag]
    memcpy(ciphertext, session->send_nonce, 12);
    memset(ciphertext + 12, 0, 4);
    memcpy(ciphertext + 16 + len, tag, 16);
    *ct_len = 16 + len + 16;

    return 0;
}
```

---

## 层次关联与映射分析

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        密码学实现安全层次映射                           │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  L7 应用层          HTTPS/TLS 1.3    SSH    Signal Protocol           │
│       ↑                    ↓              ↓                             │
│  L6 协议层         证书链验证    密钥协商    完美前向保密               │
│       ↑                    ↓              ↓                             │
│  L5 算法层         X25519      Ed25519      AES-GCM/ChaCha20          │
│       ↑                    ↓              ↓                             │
│  L4 实现层         OpenSSL 3.x    libsodium    常量时间实现           │
│       ↑                    ↓              ↓                             │
│  L3 原语层         大数运算    椭圆曲线    置换网络                     │
│       ↑                    ↓              ↓                             │
│  L2 硬件层         AES-NI    SHA-NI    RDRAND/RDSEED    ARM CE        │
│       ↑                    ↓              ↓                             │
│  L1 物理层         熵源: 抖动/热噪声/量子随机                           │
│                                                                         │
├─────────────────────────────────────────────────────────────────────────┤
│  跨层关注点: 密钥管理 | 随机数质量 | 侧信道防护 | 形式化验证           │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 决策矩阵

### 算法选择决策树

```
需要加密?
├── 是 → 对称还是非对称?
│       ├── 对称 → 需要认证加密?
│       │       ├── 是 → 有AES-NI?
│       │       │       ├── 是 → AES-256-GCM
│       │       │       └── 否 → ChaCha20-Poly1305
│       │       └── 否 → 需要流加密?
│       │               ├── 是 → ChaCha20 (不带MAC危险!)
│       │               └── 否 → AES-256-CTR + HMAC
│       └── 非对称 → 用途?
│               ├── 密钥交换 → X25519
│               ├── 数字签名 → Ed25519
│               ├── 加密小数据 → RSA-4096-OAEP
│               └── 后量子准备 → CRYSTALS-Kyber/Dilithium
├── 否 → 需要哈希?
│       ├── 是 → 需要XOF?
│       │       ├── 是 → SHAKE256
│       │       └── 否 → SHA3-256 或 BLAKE3
│       └── 否 → 需要MAC?
│               ├── 是 → HMAC-SHA256 或 KMAC256
│               └── 否 → 数字签名?
│                       ├── 是 → Ed25519
│                       └── 否 → 密码哈希?
│                               ├── 是 → Argon2id
│                               └── 否 → 无需密码学
```

### 密码学库选择矩阵

| 场景 | 推荐库 | 理由 |
|------|--------|------|
| 新Web应用 | libsodium | 现代API，防误用设计 |
| 企业系统 | OpenSSL 3.x | FIPS合规，广泛兼容 |
| IoT设备 | mbedTLS | 小体积，嵌入式优化 |
| 实时系统 | WolfSSL | 确定性延迟，实时支持 |
| AWS服务 | AWS-LC | 与AWS服务深度集成 |
| 后量子准备 | liboqs | NIST标准算法实现 |

---

## 相关资源

### 官方文档

- [OpenSSL 3.x Documentation](https://www.openssl.org/docs/man3.0/)
- [libsodium Documentation](https://doc.libsodium.org/)
- [mbedTLS Documentation](https://mbed-tls.readthedocs.io/)
- [OWASP Cryptographic Storage Cheat Sheet](https://cheatsheetseries.owasp.org/cheatsheets/Cryptographic_Storage_Cheat_Sheet.html)

### 标准规范

- NIST SP 800-57: 密钥管理建议
- NIST SP 800-131A: 算法和密钥长度过渡
- RFC 8446: TLS 1.3 规范
- RFC 7748: X25519/X448 椭圆曲线

### 学习资源

- 《Serious Cryptography》by Jean-Philippe Aumasson
- 《Real-World Cryptography》by David Wong
- Cryptography Engineering (Schneier et al.)

### 工具

- `openssl speed`: 性能基准测试
- `valgrind --tool=callgrind`: 侧信道分析
- `cryptool`: 密码学教学工具

---

## 质量验收清单

- [ ] **算法选择**: 仅使用推荐的现代算法，废弃算法已完全移除
- [ ] **密钥长度**: 对称密钥≥256位，RSA≥3072位，ECC≥256位
- [ ] **随机数质量**: 使用加密安全RNG，启动时检查熵状态
- [ ] **认证加密**: 优先使用AEAD模式 (GCM, ChaCha20-Poly1305)
- [ ] **密钥派生**: 密码使用Argon2id，密钥使用HKDF
- [ ] **常量时间**: 敏感操作使用常量时间实现
- [ ] **错误处理**: 认证失败不泄露具体原因，防止Oracle攻击
- [ ] **内存安全**: 敏感数据使用mlock，用完立即清零
- [ ] **前向保密**: 会话密钥派生支持前向保密
- [ ] **后量子准备**: 评估并规划后量子密码学迁移
- [ ] **代码审计**: 通过静态分析和专业密码学审计
- [ ] **合规检查**: 符合相关法规 (FIPS, GDPR, 等保)

---

*文档结束*
