# 密码学基础与实现

> **层级定位**: 03 System Technology Domains / 10 Security Engineering
> **参考课程**: Stanford CS255, MIT 6.857
> **难度级别**: L4-L5
> **预估学习时间**: 15-20 小时

---

## 📋 目录

- [密码学基础与实现](#密码学基础与实现)
  - [📋 目录](#-目录)
  - [1. 密码学基础](#1-密码学基础)
    - [1.1 密码学目标](#11-密码学目标)
    - [1.2 攻击模型](#12-攻击模型)
  - [2. 对称加密](#2-对称加密)
    - [2.1 AES算法框架](#21-aes算法框架)
    - [2.2 分组密码模式](#22-分组密码模式)
  - [3. 哈希函数](#3-哈希函数)
    - [3.1 SHA-256框架](#31-sha-256框架)
  - [4. 密钥交换](#4-密钥交换)
    - [4.1 Diffie-Hellman密钥交换](#41-diffie-hellman密钥交换)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. 密码学基础

### 1.1 密码学目标

```
密码学三大目标 (CIA):

┌─────────────────────────────────────────────────────────────┐
│  机密性 (Confidentiality)                                    │
│  • 信息只能被授权方访问                                      │
│  • 加密算法实现                                              │
├─────────────────────────────────────────────────────────────┤
│  完整性 (Integrity)                                          │
│  • 信息未被未授权修改                                        │
│  • 消息认证码(MAC)、数字签名                                 │
├─────────────────────────────────────────────────────────────┤
│  可用性 (Availability)                                       │
│  • 授权方能访问信息和资源                                    │
│  • 抗拒绝服务攻击                                            │
└─────────────────────────────────────────────────────────────┘

额外目标：
• 认证 (Authentication)：确认身份
• 不可否认 (Non-repudiation)：防止否认行为
```

### 1.2 攻击模型

```
攻击者能力（由弱到强）：

┌─────────────────────────────────────────────────────────────┐
│  唯密文攻击 (Ciphertext Only)                                │
│  • 攻击者只有密文                                            │
│  • 最容易防御                                                │
├─────────────────────────────────────────────────────────────┤
│  已知明文攻击 (Known Plaintext)                              │
│  • 攻击者有明文-密文对                                       │
│  • 历史上许多密码被此方式破解                                │
├─────────────────────────────────────────────────────────────┤
│  选择明文攻击 (Chosen Plaintext)                             │
│  • 攻击者可以选择明文获取密文                                │
│  • 现代加密标准假设攻击者有此能力                            │
├─────────────────────────────────────────────────────────────┤
│  选择密文攻击 (Chosen Ciphertext)                            │
│  • 攻击者可以选择密文获取明文（除目标密文外）                │
│  • 最强攻击模型                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. 对称加密

### 2.1 AES算法框架

```c
/*
 * AES (Advanced Encryption Standard)
 * • 分组大小：128位
 * • 密钥长度：128/192/256位
 * • 轮数：10/12/14轮
 */

#include <stdint.h>
#include <string.h>

// AES S盒（简化表示）
static const uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    // ... 完整256字节S盒
};

// 轮常数
static const uint8_t rcon[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

typedef struct {
    uint8_t round_key[240];
    int nr;
} aes_context_t;

// AES轮函数声明
void sub_bytes(uint8_t state[16]);
void shift_rows(uint8_t state[16]);
void mix_columns(uint8_t state[16]);
void add_round_key(uint8_t state[16], const uint8_t *round_key);

// AES加密流程
void aes_encrypt(aes_context_t *ctx, const uint8_t input[16], uint8_t output[16]) {
    uint8_t state[16];
    memcpy(state, input, 16);

    add_round_key(state, ctx->round_key);

    for (int round = 1; round < ctx->nr; round++) {
        sub_bytes(state);
        shift_rows(state);
        mix_columns(state);
        add_round_key(state, ctx->round_key + round * 16);
    }

    sub_bytes(state);
    shift_rows(state);
    add_round_key(state, ctx->round_key + ctx->nr * 16);

    memcpy(output, state, 16);
}
```

### 2.2 分组密码模式

```c
/*
 * ECB (Electronic Codebook) - 不推荐
 * • 相同明文块产生相同密文
 * • 不能隐藏数据模式
 */

/*
 * CBC (Cipher Block Chaining)
 * • 每个明文块与前一个密文块异或
 * • 需要随机IV
 */
void aes_cbc_encrypt(aes_context_t *ctx, const uint8_t *iv,
                     const uint8_t *input, size_t length,
                     uint8_t *output) {
    uint8_t block[16];
    uint8_t prev[16];
    memcpy(prev, iv, 16);

    for (size_t i = 0; i < length; i += 16) {
        for (int j = 0; j < 16; j++) {
            block[j] = input[i+j] ^ prev[j];
        }

        aes_encrypt(ctx, block, output + i);
        memcpy(prev, output + i, 16);
    }
}

/*
 * CTR (Counter) - 推荐
 * • 将分组密码转换为流密码
 * • 可以并行加密解密
 * • 随机访问密文
 */
void aes_ctr(aes_context_t *ctx, const uint8_t *nonce,
             const uint8_t *input, size_t length,
             uint8_t *output) {
    uint8_t counter[16];
    uint8_t keystream[16];
    memcpy(counter, nonce, 16);

    for (size_t i = 0; i < length; i += 16) {
        aes_encrypt(ctx, counter, keystream);

        size_t block_len = (length - i < 16) ? (length - i) : 16;
        for (size_t j = 0; j < block_len; j++) {
            output[i+j] = input[i+j] ^ keystream[j];
        }

        // 增加计数器
        for (int j = 15; j >= 0; j--) {
            if (++counter[j] != 0) break;
        }
    }
}
```

---

## 3. 哈希函数

### 3.1 SHA-256框架

```c
/*
 * SHA-256: 产生256位(32字节)哈希值
 * • 输入：任意长度消息
 * • 处理：512位块
 */

#include <stdint.h>
#include <string.h>

#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

static const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    // ... 完整64个常量
};

typedef struct {
    uint8_t data[64];
    uint32_t datalen;
    uint64_t bitlen;
    uint32_t state[8];
} sha256_context_t;

void sha256_init(sha256_context_t *ctx) {
    ctx->datalen = 0;
    ctx->bitlen = 0;
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
}

void sha256_update(sha256_context_t *ctx, const uint8_t *data, size_t len);
void sha256_final(sha256_context_t *ctx, uint8_t hash[32]);
```

---

## 4. 密钥交换

### 4.1 Diffie-Hellman密钥交换

```c
/*
 * Diffie-Hellman密钥交换
 * • 基于离散对数问题
 * • 允许双方在不安全的信道上建立共享密钥
 */

#include <stdint.h>

typedef uint64_t dh_key_t;

// 公开参数（实际使用大素数）
dh_key_t dh_p = 23;  // 素数模数
dh_key_t dh_g = 5;   // 生成元

// 快速幂取模
dh_key_t mod_exp(dh_key_t base, dh_key_t exp, dh_key_t mod) {
    dh_key_t result = 1;
    base = base % mod;

    while (exp > 0) {
        if (exp & 1)
            result = (result * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return result;
}

// Alice生成公钥
dh_key_t dh_generate_public(dh_key_t private_key) {
    return mod_exp(dh_g, private_key, dh_p);
}

// Alice计算共享密钥
dh_key_t dh_compute_shared(dh_key_t private_key, dh_key_t public_key) {
    return mod_exp(public_key, private_key, dh_p);
}
```

---

## 关联导航

### 前置知识

- [安全编码实践](./03_Secure_Coding_Practices.md)
- [内存安全](../../01_Core_Knowledge_System/02_Core_Layer/01_Memory_Model.md)

### 后续延伸

- [TLS/SSL协议实现](../15_Network_Programming/05_TLS_Implementation.md)
- [区块链密码学](./10_Blockchain_Cryptography.md)

### 参考

- Handbook of Applied Cryptography, Menezes et al.
- Cryptography Engineering, Ferguson et al.
- Serious Cryptography, Jean-Philippe Aumasson
