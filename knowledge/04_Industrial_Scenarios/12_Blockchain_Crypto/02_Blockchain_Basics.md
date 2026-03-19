# 区块链基础与C语言实现

> **目标**: 详解区块链核心概念及C语言实现基础
> **技术栈**: libsecp256k1, OpenSSL
> **难度**: L4 | **预估时间**: 4-6小时

---

## 1. 区块链核心概念

### 1.1 区块链数据结构

```text
┌─────────────────────────────────────────────────────────────┐
│                         Block N                             │
├─────────────────────────────────────────────────────────────┤
│  Block Header                                               │
│  ├── Version        (4 bytes)                               │
│  ├── Previous Hash  (32 bytes)  ← 前一区块哈希               │
│  ├── Merkle Root    (32 bytes)  ← 交易默克尔根               │
│  ├── Timestamp      (4 bytes)                               │
│  ├── Difficulty     (4 bytes)                               │
│  └── Nonce          (4 bytes)   ← 工作量证明                 │
├─────────────────────────────────────────────────────────────┤
│  Transactions                                               │
│  ├── TX1: 输入 → 输出                                        │
│  ├── TX2: 输入 → 输出                                        │
│  └── ...                                                    │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                       Block N+1                             │
├─────────────────────────────────────────────────────────────┤
│  Previous Hash = Hash(Block N Header)                       │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 C语言区块结构定义

```c
#include <stdint.h>
#include <time.h>

#define HASH_SIZE 32
#define BLOCK_HEADER_SIZE 80

// 区块头
typedef struct {
    uint32_t version;           // 版本号
    uint8_t prev_hash[HASH_SIZE];  // 前一区块哈希
    uint8_t merkle_root[HASH_SIZE]; // 默克尔根
    uint32_t timestamp;         // 时间戳
    uint32_t bits;             // 难度目标
    uint32_t nonce;            // 随机数
} BlockHeader;

// 交易输入
typedef struct {
    uint8_t tx_hash[HASH_SIZE];  // 引用交易的哈希
    uint32_t index;              // 输出索引
    uint8_t* script_sig;         // 解锁脚本
    uint32_t script_len;
    uint32_t sequence;           // 序列号
} TxInput;

// 交易输出
typedef struct {
    uint64_t value;              // 金额 (聪)
    uint8_t* script_pubkey;      // 锁定脚本
    uint32_t script_len;
} TxOutput;

// 交易
typedef struct {
    uint32_t version;
    uint32_t input_count;
    TxInput* inputs;
    uint32_t output_count;
    TxOutput* outputs;
    uint32_t lock_time;
} Transaction;

// 完整区块
typedef struct {
    BlockHeader header;
    uint32_t tx_count;
    Transaction* transactions;
} Block;
```

---


---

## 📑 目录

- [区块链基础与C语言实现](#区块链基础与c语言实现)
  - [1. 区块链核心概念](#1-区块链核心概念)
    - [1.1 区块链数据结构](#11-区块链数据结构)
    - [1.2 C语言区块结构定义](#12-c语言区块结构定义)
  - [📑 目录](#-目录)
  - [2. 哈希与工作量证明](#2-哈希与工作量证明)
    - [2.1 双SHA-256哈希](#21-双sha-256哈希)
    - [2.2 工作量证明挖矿](#22-工作量证明挖矿)
  - [3. 默克尔树](#3-默克尔树)
    - [3.1 默克尔树构建](#31-默克尔树构建)
  - [4. 数字签名与椭圆曲线](#4-数字签名与椭圆曲线)
    - [4.1 使用libsecp256k1](#41-使用libsecp256k1)
  - [5. 地址生成](#5-地址生成)
    - [5.1 比特币地址生成](#51-比特币地址生成)
  - [6. 简单区块链实现](#6-简单区块链实现)
  - [7. 安全注意事项](#7-安全注意事项)
    - [7.1 常见攻击防护](#71-常见攻击防护)
    - [7.2 密钥管理最佳实践](#72-密钥管理最佳实践)


---

## 2. 哈希与工作量证明

### 2.1 双SHA-256哈希

```c
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <string.h>

// 双SHA256 (比特币标准)
void double_sha256(const uint8_t* data, size_t len, uint8_t hash[32]) {
    uint8_t tmp[32];
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();

    // 第一次SHA256
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, data, len);
    EVP_DigestFinal_ex(ctx, tmp, NULL);

    // 第二次SHA256
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, tmp, 32);
    EVP_DigestFinal_ex(ctx, hash, NULL);

    EVP_MD_CTX_free(ctx);
}

// 区块头哈希
void block_hash(const BlockHeader* header, uint8_t hash[32]) {
    uint8_t serialized[BLOCK_HEADER_SIZE];

    // 序列化区块头 (小端序)
    uint32_t offset = 0;
    memcpy(serialized + offset, &header->version, 4); offset += 4;
    memcpy(serialized + offset, header->prev_hash, 32); offset += 32;
    memcpy(serialized + offset, header->merkle_root, 32); offset += 32;
    memcpy(serialized + offset, &header->timestamp, 4); offset += 4;
    memcpy(serialized + offset, &header->bits, 4); offset += 4;
    memcpy(serialized + offset, &header->nonce, 4);

    double_sha256(serialized, BLOCK_HEADER_SIZE, hash);
}
```

### 2.2 工作量证明挖矿

```c
#include <stdbool.h>
#include <stdio.h>

// 难度目标检查 (简化版)
bool check_proof_of_work(const uint8_t hash[32], uint32_t bits) {
    // 将compact bits展开为256位目标
    uint32_t exp = bits >> 24;
    uint32_t mant = bits & 0x007fffff;

    // 简单检查: 哈希值小于目标
    // 实际实现需要完整的256位比较
    for (int i = 0; i < 32; i++) {
        uint8_t target_byte = (exp > 32 - i) ? 0 :
                              (exp == 32 - i) ? (mant >> (8 * (2 - (32 - exp - i)))) & 0xff : 0xff;
        if (hash[i] < target_byte) return true;
        if (hash[i] > target_byte) return false;
    }
    return true;
}

// 挖矿
uint32_t mine_block(BlockHeader* header, uint32_t max_nonce) {
    uint8_t hash[32];

    for (uint32_t nonce = 0; nonce < max_nonce; nonce++) {
        header->nonce = nonce;
        block_hash(header, hash);

        if (check_proof_of_work(hash, header->bits)) {
            printf("Found valid nonce: %u\n", nonce);
            printf("Hash: ");
            for (int i = 0; i < 32; i++) printf("%02x", hash[i]);
            printf("\n");
            return nonce;
        }
    }

    return 0; // 未找到
}
```

---

## 3. 默克尔树

### 3.1 默克尔树构建

```c
// 计算两个哈希的父哈希
void hash_pair(const uint8_t left[32], const uint8_t right[32],
               uint8_t parent[32]) {
    uint8_t combined[64];
    memcpy(combined, left, 32);
    memcpy(combined + 32, right, 32);
    double_sha256(combined, 64, parent);
}

// 构建默克尔根 (简化版 - 假设2^n个交易)
void build_merkle_root(uint8_t** tx_hashes, int tx_count,
                       uint8_t root[32]) {
    if (tx_count == 0) {
        memset(root, 0, 32);
        return;
    }
    if (tx_count == 1) {
        memcpy(root, tx_hashes[0], 32);
        return;
    }

    // 计算下一层哈希数
    int next_count = (tx_count + 1) / 2;
    uint8_t** next_level = malloc(next_count * sizeof(uint8_t*));

    for (int i = 0; i < next_count; i++) {
        next_level[i] = malloc(32);
        uint8_t* left = tx_hashes[i * 2];
        uint8_t* right = (i * 2 + 1 < tx_count) ?
                         tx_hashes[i * 2 + 1] : left;
        hash_pair(left, right, next_level[i]);
    }

    // 递归构建
    build_merkle_root(next_level, next_count, root);

    // 清理
    for (int i = 0; i < next_count; i++) free(next_level[i]);
    free(next_level);
}
```

---

## 4. 数字签名与椭圆曲线

### 4.1 使用libsecp256k1

```c
#include <secp256k1.h>

// 生成密钥对
int generate_keypair(uint8_t private_key[32], uint8_t public_key[33]) {
    secp256k1_context* ctx = secp256k1_context_create(
        SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);

    // 生成随机私钥
    if (!secure_random(private_key, 32)) {
        return -1;
    }

    // 验证私钥有效性
    if (!secp256k1_ec_seckey_verify(ctx, private_key)) {
        return -1;
    }

    // 计算公钥
    secp256k1_pubkey pubkey;
    if (!secp256k1_ec_pubkey_create(ctx, &pubkey, private_key)) {
        return -1;
    }

    // 序列化公钥 (压缩格式)
    size_t len = 33;
    secp256k1_ec_pubkey_serialize(ctx, public_key, &len, &pubkey,
                                   SECP256K1_EC_COMPRESSED);

    secp256k1_context_destroy(ctx);
    return 0;
}

// 签名消息
int sign_message(const uint8_t private_key[32],
                 const uint8_t msg_hash[32],
                 uint8_t signature[64]) {
    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);

    secp256k1_ecdsa_signature sig;
    if (!secp256k1_ecdsa_sign(ctx, &sig, msg_hash, private_key,
                               NULL, NULL)) {
        secp256k1_context_destroy(ctx);
        return -1;
    }

    // 序列化签名
    secp256k1_ecdsa_signature_serialize_compact(ctx, signature, &sig);

    secp256k1_context_destroy(ctx);
    return 0;
}

// 验证签名
int verify_signature(const uint8_t public_key[33],
                     const uint8_t msg_hash[32],
                     const uint8_t signature[64]) {
    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);

    // 解析公钥
    secp256k1_pubkey pubkey;
    if (!secp256k1_ec_pubkey_parse(ctx, &pubkey, public_key, 33)) {
        secp256k1_context_destroy(ctx);
        return -1;
    }

    // 解析签名
    secp256k1_ecdsa_signature sig;
    if (!secp256k1_ecdsa_signature_parse_compact(ctx, &sig, signature)) {
        secp256k1_context_destroy(ctx);
        return -1;
    }

    // 验证
    int result = secp256k1_ecdsa_verify(ctx, &sig, msg_hash, &pubkey);

    secp256k1_context_destroy(ctx);
    return result;
}
```

---

## 5. 地址生成

### 5.1 比特币地址生成

```c
// RIPEMD160(SHA256(pubkey))
void hash160(const uint8_t* data, size_t len, uint8_t hash[20]) {
    uint8_t sha256_hash[32];
    SHA256(data, len, sha256_hash);

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_ripemd160(), NULL);
    EVP_DigestUpdate(ctx, sha256_hash, 32);

    unsigned int hash_len;
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);
}

// Base58编码
static const char BASE58_CHARS[] =
    "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

int base58_encode(const uint8_t* data, size_t len, char* out, size_t out_len) {
    // 计算前导零
    int zeros = 0;
    while (zeros < len && data[zeros] == 0) zeros++;

    // 大数转换
    uint8_t* buf = malloc(len * 2);  // 足够大
    memset(buf, 0, len * 2);

    for (size_t i = zeros; i < len; i++) {
        uint16_t carry = data[i];
        for (size_t j = 0; j < len * 2; j++) {
            carry += (uint16_t)buf[j] << 8;
            buf[j] = carry % 58;
            carry /= 58;
        }
    }

    // 跳过前导零
    size_t j = len * 2;
    while (j > 0 && buf[j-1] == 0) j--;

    // 编码
    if (out_len < (size_t)zeros + j + 1) {
        free(buf);
        return -1;
    }

    size_t k = 0;
    for (int i = 0; i < zeros; i++) out[k++] = '1';
    while (j > 0) out[k++] = BASE58_CHARS[buf[--j]];
    out[k] = '\0';

    free(buf);
    return 0;
}

// 生成P2PKH地址
void generate_address(const uint8_t public_key[33], char address[35]) {
    // 1. Hash160
    uint8_t hash[20];
    hash160(public_key, 33, hash);

    // 2. 添加版本字节 (0x00 for mainnet)
    uint8_t versioned[21];
    versioned[0] = 0x00;
    memcpy(versioned + 1, hash, 20);

    // 3. 计算校验和 (双SHA256前4字节)
    uint8_t checksum[32];
    double_sha256(versioned, 21, checksum);

    // 4. 组合
    uint8_t binary_addr[25];
    memcpy(binary_addr, versioned, 21);
    memcpy(binary_addr + 21, checksum, 4);

    // 5. Base58编码
    base58_encode(binary_addr, 25, address, 35);
}
```

---

## 6. 简单区块链实现

```c
// 简化版区块链
#define MAX_BLOCKS 1000

typedef struct {
    Block blocks[MAX_BLOCKS];
    int count;
    uint32_t difficulty;
} Blockchain;

// 初始化创世区块
void init_blockchain(Blockchain* chain) {
    chain->count = 0;
    chain->difficulty = 0x1d00ffff; // 比特币创世难度

    // 创建创世区块
    Block* genesis = &chain->blocks[0];
    memset(&genesis->header, 0, sizeof(BlockHeader));
    genesis->header.version = 1;
    genesis->header.timestamp = 1231006505; // 2009-01-03
    genesis->header.bits = chain->difficulty;

    // 设置创世交易
    genesis->tx_count = 1;
    genesis->transactions = malloc(sizeof(Transaction));
    // ... 初始化 coinbase 交易

    chain->count = 1;
}

// 添加新区块
int add_block(Blockchain* chain, Block* new_block) {
    if (chain->count >= MAX_BLOCKS) return -1;

    // 验证前一区块哈希
    Block* prev = &chain->blocks[chain->count - 1];
    uint8_t prev_hash[32];
    block_hash(&prev->header, prev_hash);

    if (memcmp(new_block->header.prev_hash, prev_hash, 32) != 0) {
        return -1; // 前一区块哈希不匹配
    }

    // 验证工作量证明
    uint8_t new_hash[32];
    block_hash(&new_block->header, new_hash);
    if (!check_proof_of_work(new_hash, new_block->header.bits)) {
        return -1; // 工作量证明无效
    }

    // 添加区块
    memcpy(&chain->blocks[chain->count], new_block, sizeof(Block));
    chain->count++;

    return 0;
}
```

---

## 7. 安全注意事项

### 7.1 常见攻击防护

```
攻击类型                防护措施
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
51%攻击                增加确认数，监控算力分布
双花攻击               等待多确认，检查交易池
重放攻击               添加链ID到签名
日食攻击               多节点连接，随机节点选择
自私挖矿               奖励机制设计，监控出块时间
```

### 7.2 密钥管理最佳实践

```c
// 分层确定性钱包 (HD Wallet) 简化版
void derive_key(const uint8_t master_seed[64],
                const uint32_t* path, int depth,
                uint8_t private_key[32]) {
    uint8_t chain_code[32];
    memcpy(private_key, master_seed, 32);
    memcpy(chain_code, master_seed + 32, 32);

    for (int i = 0; i < depth; i++) {
        // HMAC-SHA512(child_index || private_key || chain_code)
        uint8_t data[37];
        data[0] = 0x00;
        memcpy(data + 1, private_key, 32);
        memcpy(data + 33, &path[i], 4);

        uint8_t hmac[64];
        HMAC(EVP_sha512(), chain_code, 32, data, 37, hmac, NULL);

        memcpy(private_key, hmac, 32);
        memcpy(chain_code, hmac + 32, 32);
    }
}
```

---

> **核心洞察**: 区块链的核心价值在于去中心化共识和密码学安全。C语言实现需要格外注意内存安全、时序攻击防护和随机数质量。

---

*最后更新: 2026-03-14*


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
