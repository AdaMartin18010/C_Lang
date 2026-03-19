# C语言区块链与密码学实现

> **层级定位**: 04_Industrial_Scenarios > 12_Blockchain_Crypto
> **难度级别**: L4-L5 (高级到专家)
> **前置知识**: [位运算](../../01_Core_Knowledge_System/01_Basic_Layer/03_Operators_Expressions.md), [内存管理](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md), [安全编码](../../01_Core_Knowledge_System/09_Safety_Standards/)
> **预估学习时间**: 15-20小时

---

## 模块概述

区块链技术和密码学是现代信息安全的基石。本模块深入讲解如何用C语言实现核心密码学算法和简化版区块链系统。从哈希函数到智能合约虚拟机，帮助你理解去中心化技术的底层原理。

### 为什么用C实现？

- **性能**: 密码学运算需要最高性能
- **可移植性**: 从嵌入式到服务器全平台支持
- **资源控制**: 精确控制内存和计算资源
- **审计友好**: 代码清晰，便于安全审计

---

## 学习目标

完成本模块后，你将能够：

1. **实现核心哈希算法**: SHA-256, Keccak-256
2. **理解非对称加密**: RSA和ECDSA原理与实现
3. **构建简化区块链**: 区块结构、POW共识、P2P网络
4. **编写智能合约VM**: 栈式虚拟机设计与实现
5. **应用安全实践**: 防止侧信道攻击、安全密钥管理

---

## 内容导航

### 核心内容

| 主题 | 文件 | 难度 | 时间 |
|:-----|:-----|:----:|:----:|
| 密码学原语 | [01_Cryptographic_Primitives.md](./01_Cryptographic_Primitives.md) | ⭐⭐⭐⭐ | 4h |
| 区块链基础 | [02_Blockchain_Basics.md](./02_Blockchain_Basics.md) | ⭐⭐⭐⭐ | 5h |
| 高级加密技术 | [03_Advanced_Cryptography.md](./03_Advanced_Cryptography.md) | ⭐⭐⭐⭐⭐ | 4h |
| 智能合约VM | [04_Smart_Contract_VM.md](./04_Smart_Contract_VM.md) | ⭐⭐⭐⭐⭐ | 5h |

### 实践项目

- [项目1: 完整区块链节点](./projects/project1_full_node.md)
- [项目2: 加密货币钱包](./projects/project2_wallet.md)
- [项目3: 智能合约部署](./projects/project3_contract_deployment.md)

---

## 一、密码学基础

### 1.1 哈希函数 SHA-256

SHA-256是比特币和许多区块链系统的核心哈希算法。

```c
/**
 * @file sha256.c
 * @brief SHA-256哈希算法实现
 */

#include <stdint.h>
#include <string.h>

#define SHA256_BLOCK_SIZE 64
#define SHA256_DIGEST_SIZE 32

typedef struct {
    uint32_t state[8];
    uint64_t bitcount;
    uint8_t buffer[SHA256_BLOCK_SIZE];
    size_t buffer_len;
} sha256_ctx;

// 初始化SHA-256上下文
void sha256_init(sha256_ctx *ctx) {
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
    ctx->bitcount = 0;
    ctx->buffer_len = 0;
}

// 完整的SHA-256实现...
// (详见完整代码文件)

// 使用示例
void sha256_example(void) {
    sha256_ctx ctx;
    uint8_t digest[SHA256_DIGEST_SIZE];
    const char *message = "Hello, Blockchain!";

    sha256_init(&ctx);
    sha256_update(&ctx, (uint8_t*)message, strlen(message));
    sha256_final(&ctx, digest);

    // 打印哈希值
    printf("SHA-256: ");
    for (int i = 0; i < SHA256_DIGEST_SIZE; i++) {
        printf("%02x", digest[i]);
    }
    printf("\n");
}
```

### 1.2 默克尔树 (Merkle Tree)

默克尔树是区块链中用于高效验证大量数据的关键数据结构。

```c
/**
 * @brief 默克尔树节点
 */
typedef struct merkle_node {
    uint8_t hash[SHA256_DIGEST_SIZE];
    struct merkle_node *left;
    struct merkle_node *right;
    struct merkle_node *parent;
} merkle_node;

/**
 * @brief 构建默克尔树
 */
merkle_node* build_merkle_tree(uint8_t **data, size_t data_count, size_t data_len);

/**
 * @brief 验证默克尔证明
 */
bool verify_merkle_proof(uint8_t *root_hash, uint8_t *leaf_hash,
                         uint8_t **proof, size_t proof_len, size_t leaf_index);
```

---

## 二、区块链核心

### 2.1 区块结构

```c
/**
 * @brief 区块头结构 (80字节)
 */
typedef struct {
    uint32_t version;           // 版本号
    uint8_t prev_block[32];     // 前一区块哈希
    uint8_t merkle_root[32];    // 默克尔根
    uint32_t timestamp;         // 时间戳
    uint32_t bits;              // 难度目标
    uint32_t nonce;             // 随机数
} block_header;

/**
 * @brief 完整区块
 */
typedef struct {
    block_header header;
    uint32_t tx_count;
    transaction *transactions;
} block;

/**
 * @brief 计算区块哈希
 */
void compute_block_hash(const block_header *header, uint8_t *hash_out);

/**
 * @brief 验证区块POW
 */
bool verify_pow(const block_header *header, uint32_t difficulty);
```

### 2.2 工作量证明 (Proof of Work)

```c
/**
 * @brief 挖矿：寻找满足难度的nonce
 */
bool mine_block(block_header *header, uint32_t difficulty, uint32_t max_attempts) {
    uint8_t hash[SHA256_DIGEST_SIZE];
    uint32_t target = 0xFFFFFFFF >> difficulty;

    for (uint32_t nonce = 0; nonce < max_attempts; nonce++) {
        header->nonce = nonce;
        compute_block_hash(header, hash);

        // 检查是否满足难度要求 (简化版)
        uint32_t hash_prefix = (hash[0] << 24) | (hash[1] << 16) |
                               (hash[2] << 8) | hash[3];
        if (hash_prefix < target) {
            return true;  // 找到有效nonce
        }
    }

    return false;  // 未找到
}
```

### 2.3 简化版区块链实现

```c
/**
 * @brief 区块链结构
 */
typedef struct {
    block *blocks;
    size_t block_count;
    size_t capacity;
    uint32_t difficulty;
    mempool *tx_pool;
} blockchain;

/**
 * @brief 初始化区块链
 */
blockchain* blockchain_create(uint32_t difficulty);

/**
 * @brief 添加新区块
 */
bool blockchain_add_block(blockchain *chain, block *new_block);

/**
 * @brief 验证整个链
 */
bool blockchain_validate(const blockchain *chain);

/**
 * @brief 查找交易
 */
transaction* blockchain_find_transaction(const blockchain *chain,
                                          const uint8_t *tx_hash);
```

---

## 三、非对称加密与数字签名

### 3.1 ECDSA (椭圆曲线数字签名算法)

ECDSA是比特币使用的签名算法。

```c
/**
 * @brief 椭圆曲线点
 */
typedef struct {
    uint8_t x[32];
    uint8_t y[32];
} ec_point;

/**
 * @brief ECDSA密钥对
 */
typedef struct {
    uint8_t private_key[32];    // 私钥
    ec_point public_key;        // 公钥
} ecdsa_keypair;

/**
 * @brief ECDSA签名
 */
typedef struct {
    uint8_t r[32];
    uint8_t s[32];
} ecdsa_signature;

/**
 * @brief 生成密钥对
 */
void ecdsa_generate_keypair(ecdsa_keypair *keypair);

/**
 * @brief 签名消息
 */
void ecdsa_sign(const ecdsa_keypair *keypair, const uint8_t *message,
                size_t msg_len, ecdsa_signature *signature);

/**
 * @brief 验证签名
 */
bool ecdsa_verify(const ec_point *public_key, const uint8_t *message,
                  size_t msg_len, const ecdsa_signature *signature);
```

### 3.2 钱包实现

```c
/**
 * @brief 简化版钱包
 */
typedef struct {
    char address[35];           // Base58编码地址
    ecdsa_keypair keypair;
    uint64_t balance;
} wallet;

/**
 * @brief 从公钥生成地址
 */
void generate_address(const ec_point *public_key, char *address_out);

/**
 * @brief 创建交易
 */
transaction* wallet_create_transaction(const wallet *from,
                                        const char *to_address,
                                        uint64_t amount,
                                        uint64_t fee);
```

---

## 四、智能合约虚拟机

### 4.1 指令集架构

```c
/**
 * @brief 虚拟机操作码
 */
typedef enum {
    OP_STOP = 0x00,      // 停止执行
    OP_PUSH = 0x01,      // 压栈
    OP_POP = 0x02,       // 出栈
    OP_ADD = 0x10,       // 加法
    OP_SUB = 0x11,       // 减法
    OP_MUL = 0x12,       // 乘法
    OP_DIV = 0x13,       // 除法
    OP_EQ = 0x20,        // 等于
    OP_LT = 0x21,        // 小于
    OP_GT = 0x22,        // 大于
    OP_JMP = 0x30,       // 跳转
    OP_JMPIF = 0x31,     // 条件跳转
    OP_CALL = 0x40,      // 调用合约
    OP_STORAGE_GET = 0x50,  // 读取存储
    OP_STORAGE_SET = 0x51,  // 写入存储
} opcode;
```

### 4.2 虚拟机实现

```c
/**
 * @brief 智能合约VM
 */
typedef struct {
    uint8_t *code;          // 字节码
    size_t code_size;
    uint64_t *stack;        // 执行栈
    size_t stack_size;
    size_t sp;              // 栈指针
    size_t pc;              // 程序计数器
    uint64_t gas;           // 剩余gas
    storage *contract_storage;
} sc_vm;

/**
 * @brief 创建VM实例
 */
sc_vm* vm_create(const uint8_t *code, size_t code_size, uint64_t gas_limit);

/**
 * @brief 执行字节码
 */
int vm_execute(sc_vm *vm);

/**
 * @brief 销毁VM
 */
void vm_destroy(sc_vm *vm);
```

---

## 五、安全考虑

### 5.1 侧信道攻击防护

```c
/**
 * @brief 恒定时间比较 (防止时序攻击)
 */
int constant_time_compare(const uint8_t *a, const uint8_t *b, size_t len) {
    uint8_t result = 0;
    for (size_t i = 0; i < len; i++) {
        result |= a[i] ^ b[i];
    }
    return result;  // 0表示相等，非0表示不等
}
```

### 5.2 安全随机数生成

```c
/**
 * @brief 加密安全随机数
 */
bool secure_random(uint8_t *buffer, size_t len);
```

---

## 六、实践练习

### 练习1: 实现自己的简化版比特币

目标: 创建一个功能完整的简化版加密货币，包含：

- 基本的P2P网络通信
- 钱包功能
- 转账交易
- 简单的挖矿

### 练习2: 性能优化

优化SHA-256实现，目标达到OpenSSL速度的80%。

### 练习3: 安全审计

审查提供的代码，找出至少3个潜在的安全问题。

---

## 七、延伸阅读

### 推荐资源

1. **"Mastering Bitcoin"** - Andreas Antonopoulos
   - 比特币技术详解

2. **"Mastering Ethereum"** - Andreas Antonopoulos
   - 以太坊和智能合约

3. **Bitcoin白皮书** - Satoshi Nakamoto
   - 区块链的开创性论文

4. **"Cryptography Engineering"** - Ferguson, Schneier, Kohno
   - 密码学工程实践

### 开源项目

- [Bitcoin Core](https://github.com/bitcoin/bitcoin)
- [libsecp256k1](https://github.com/bitcoin-core/secp256k1) - 优化ECDSA库
- [OpenSSL](https://github.com/openssl/openssl) - 通用密码学库

---

> **最后更新**: 2026-03-19
> **维护者**: AI Code Review
> **审核状态**: 已完成深化
