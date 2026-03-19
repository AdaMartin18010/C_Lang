# 硬件安全 (Hardware Security)

> **层级定位**: 03_System_Technology_Domains > 07_Hardware_Security
> **难度级别**: ⭐⭐⭐⭐⭐ L5
> **前置知识**: 计算机体系结构、密码学基础、硬件设计
> **参考标准**: ARM TrustZone, Intel SGX, TPM 2.0 Specification

---

## 概述

硬件安全是计算机系统安全的根基，通过在硬件层面实现安全机制，提供比软件安全更高的保护级别。本目录涵盖可信执行环境、硬件加密、安全启动、侧信道防护等关键技术。

---

## 核心领域

| 技术 | 描述 | 典型应用 |
|:-----|:-----|:---------|
| **可信执行环境 (TEE)** | ARM TrustZone, Intel SGX | 移动支付、DRM |
| **安全启动 (Secure Boot)** | 链式信任验证 | 嵌入式系统、服务器 |
| **硬件加密引擎** | AES/SHA/RSA加速器 | 数据加密、TLS加速 |
| **物理不可克隆 (PUF)** | 利用制造变异 | 设备唯一标识 |
| **侧信道防护** | 时序/功耗/电磁防护 | 密码学实现 |

---

## 深入理解

### 1. ARM TrustZone架构

ARM TrustZone将系统分为安全世界(Normal World)和非安全世界(Secure World)。

```
┌─────────────────────────────────────────────────────────────┐
│                      非安全世界 (Normal World)               │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐ │
│  │  Rich OS    │  │  应用程序   │  │  非安全驱动          │ │
│  │ (Linux/RTOS)│  │             │  │                     │ │
│  └──────┬──────┘  └─────────────┘  └─────────────────────┘ │
├─────────┼───────────────────────────────────────────────────┤
│  Monitor Mode (安全状态切换)                                 │
├─────────┼───────────────────────────────────────────────────┤
│                      安全世界 (Secure World)                 │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐ │
│  │  Trusted OS │  │  安全服务   │  │  安全驱动            │ │
│  │ (OP-TEE)    │  │ (密钥管理)  │  │ (加密引擎访问)       │ │
│  └─────────────┘  └─────────────┘  └─────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

#### TrustZone内存隔离

```c
// TrustZone地址空间控制
#define TZASC_BASE  0x1E000000

// 配置TrustZone地址空间控制器
void tzasc_configure_region(uint32_t region, uint32_t start, uint32_t end, uint32_t ns) {
    volatile uint32_t* tzasc = (uint32_t*)TZASC_BASE;

    // 区域基地址
    tzasc[0x100 + region * 8] = start;
    // 区域顶层地址
    tzasc[0x104 + region * 8] = end;
    // 区域属性 (NS=1为非安全区域)
    tzasc[0x108 + region * 8] = ns ? 0x1 : 0x0;
}

// 初始化TrustZone内存映射
void trustzone_memory_init(void) {
    // 区域0: 0x00000000 - 0x3FFFFFFF (1GB DDR, 非安全)
    tzasc_configure_region(0, 0x00000000, 0x3FFFFFFF, 1);

    // 区域1: 0x40000000 - 0x47FFFFFF (128MB DDR, 安全)
    tzasc_configure_region(1, 0x40000000, 0x47FFFFFF, 0);

    // 使能TZASC
    volatile uint32_t* tzasc_control = (uint32_t*)(TZASC_BASE + 0x00);
    *tzasc_control = 0x1;
}
```

#### SMC调用 (Secure Monitor Call)

```c
// SMC调用接口 (从非安全世界调用安全世界服务)

// SMC调用编号定义
#define SMC_FNID_TA_SESSION_OPEN    0xFF000001
#define SMC_FNID_TA_INVOKE_CMD      0xFF000002
#define SMC_FNID_TA_SESSION_CLOSE   0xFF000003

// 执行SMC调用
uint32_t smc_call(uint32_t fn_id, uint32_t a1, uint32_t a2,
                  uint32_t a3, uint32_t a4, uint32_t* ret_a1, uint32_t* ret_a2) {
    register uint32_t r0 __asm__("r0") = fn_id;
    register uint32_t r1 __asm__("r1") = a1;
    register uint32_t r2 __asm__("r2") = a2;
    register uint32_t r3 __asm__("r3") = a3;
    register uint32_t r4 __asm__("r4") = a4;

    __asm__ volatile (
        "smc #0"
        : "=r" (r0), "=r" (r1), "=r" (r2)
        : "r" (r0), "r" (r1), "r" (r2), "r" (r3), "r" (r4)
        : "memory"
    );

    *ret_a1 = r1;
    *ret_a2 = r2;
    return r0;  // 返回状态码
}

// 调用安全存储服务示例
int secure_storage_write(uint32_t obj_id, const uint8_t* data, size_t len) {
    // 共享内存物理地址 (非安全世界映射)
    uint32_t shared_mem_pa = 0x40000000;  // 安全区域共享内存

    // 拷贝数据到共享内存
    memcpy((void*)shared_mem_pa, data, len);

    uint32_t ret1, ret2;
    uint32_t result = smc_call(
        SMC_FNID_TA_INVOKE_CMD,
        obj_id,           // 对象ID
        shared_mem_pa,    // 数据物理地址
        len,              // 数据长度
        0x0001,           // 命令: WRITE
        &ret1, &ret2
    );

    return result == 0 ? 0 : -1;
}
```

### 2. 安全启动 (Secure Boot)

安全启动通过链式信任确保系统启动过程中每个组件的完整性。

```
信任链 (Chain of Trust):
┌──────────────────────────────────────────────────────────────┐
│  ROM Code (BootROM) - Root of Trust                          │
│  ↓ 验证下一级                                                │
│  Bootloader (SPL/U-Boot) - 第一阶段引导                      │
│  ↓ 验证下一级                                                │
│  Trusted Firmware (ATF/OP-TEE) - 安全固件                    │
│  ↓ 验证下一级                                                │
│  Operating System Kernel - 操作系统内核                      │
│  ↓ 验证下一级                                                │
│  Applications - 应用程序                                     │
└──────────────────────────────────────────────────────────────┘
```

#### 镜像签名与验证

```c
#include <mbedtls/rsa.h>
#include <mbedtls/sha256.h>

// RSA-2048公钥 (用于验证)
static const uint8_t root_public_key[] = {
    0x00, 0x01, 0x02, 0x03, // 模数 n (256字节)
    // ...
};

// 镜像头结构
typedef struct {
    uint32_t magic;           // 'IMGE'
    uint32_t version;
    uint32_t image_size;
    uint32_t load_addr;
    uint32_t entry_point;
    uint8_t  hash[32];        // SHA-256 of image
    uint8_t  signature[256];  // RSA-PSS signature
} ImageHeader;

// 验证镜像签名
int verify_image_signature(const uint8_t* image, size_t image_size,
                           const uint8_t* signature, size_t sig_len,
                           const uint8_t* public_key) {
    mbedtls_rsa_context rsa;
    mbedtls_rsa_init(&rsa, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256);

    // 导入公钥
    mbedtls_mpi_read_binary(&rsa.N, public_key, 256);
    mbedtls_mpi_read_binary(&rsa.E, public_key + 256, 3);
    rsa.len = 256;

    // 计算镜像哈希
    uint8_t hash[32];
    mbedtls_sha256_context sha_ctx;
    mbedtls_sha256_init(&sha_ctx);
    mbedtls_sha256_starts(&sha_ctx, 0);
    mbedtls_sha256_update(&sha_ctx, image, image_size);
    mbedtls_sha256_finish(&sha_ctx, hash);

    // 验证RSA-PSS签名
    int ret = mbedtls_rsa_rsassa_pss_verify(&rsa, NULL, NULL,
                                            MBEDTLS_RSA_PUBLIC,
                                            MBEDTLS_MD_SHA256,
                                            32, hash, signature);

    mbedtls_rsa_free(&rsa);
    return ret == 0 ? 0 : -1;
}

// 安全启动主流程
int secure_boot_flow(void) {
    // 1. 从存储介质加载镜像头
    ImageHeader header;
    load_image_header(&header);

    // 2. 验证镜像头魔数
    if (header.magic != 0x45474D49) {  // 'IMGE'
        return -1;
    }

    // 3. 加载镜像数据
    uint8_t* image_data = (uint8_t*)header.load_addr;
    load_image_data(&header, image_data);

    // 4. 验证签名
    if (verify_image_signature(image_data, header.image_size,
                               header.signature, 256, root_public_key) != 0) {
        // 验证失败，进入恢复模式
        enter_recovery_mode();
        return -1;
    }

    // 5. 验证哈希
    uint8_t computed_hash[32];
    mbedtls_sha256(image_data, header.image_size, computed_hash, 0);
    if (memcmp(computed_hash, header.hash, 32) != 0) {
        return -1;
    }

    // 6. 跳转到入口点
    void (*entry_point)(void) = (void (*)(void))header.entry_point;
    entry_point();

    return 0;
}
```

### 3. 硬件加密引擎

现代SoC集成专用硬件加速器处理AES、SHA、RSA等算法。

```c
// 硬件AES引擎接口 (以ARM Cryptocell为例)
#define AES_ENGINE_BASE  0x10001000

// AES寄存器偏移
#define AES_CONTROL      0x00
#define AES_STATUS       0x04
#define AES_KEY_0        0x10
#define AES_IV_0         0x30
#define AES_DATA_IN_0    0x40
#define AES_DATA_OUT_0   0x50

// AES模式
#define AES_MODE_ECB     0x00
#define AES_MODE_CBC     0x01
#define AES_MODE_CTR     0x02
#define AES_MODE_GCM     0x03

typedef struct {
    uint8_t key[32];      // 支持128/192/256位密钥
    uint8_t iv[16];       // 初始化向量 (CBC/CTR模式)
    uint8_t key_size;     // 16/24/32字节
} AESContext;

// 初始化AES硬件
void hw_aes_init(const AESContext* ctx) {
    volatile uint32_t* aes = (uint32_t*)AES_ENGINE_BASE;

    // 等待引擎空闲
    while (aes[AES_STATUS / 4] & 0x1);

    // 配置密钥 (256位)
    for (int i = 0; i < ctx->key_size / 4; i++) {
        aes[(AES_KEY_0 + i*4) / 4] =
            (ctx->key[i*4] << 0) | (ctx->key[i*4+1] << 8) |
            (ctx->key[i*4+2] << 16) | (ctx->key[i*4+3] << 24);
    }

    // 配置IV (CBC模式)
    for (int i = 0; i < 4; i++) {
        aes[(AES_IV_0 + i*4) / 4] =
            (ctx->iv[i*4] << 0) | (ctx->iv[i*4+1] << 8) |
            (ctx->iv[i*4+2] << 16) | (ctx->iv[i*4+3] << 24);
    }
}

// 硬件AES加密单块
void hw_aes_encrypt_block(const uint8_t* input, uint8_t* output) {
    volatile uint32_t* aes = (uint32_t*)AES_ENGINE_BASE;

    // 写入输入数据
    for (int i = 0; i < 4; i++) {
        aes[(AES_DATA_IN_0 + i*4) / 4] =
            (input[i*4] << 0) | (input[i*4+1] << 8) |
            (input[i*4+2] << 16) | (input[i*4+3] << 24);
    }

    // 启动加密
    aes[AES_CONTROL / 4] = 0x1;  // ENCRYPT bit

    // 等待完成
    while (aes[AES_STATUS / 4] & 0x1);

    // 读取输出
    for (int i = 0; i < 4; i++) {
        uint32_t word = aes[(AES_DATA_OUT_0 + i*4) / 4];
        output[i*4]   = word & 0xFF;
        output[i*4+1] = (word >> 8) & 0xFF;
        output[i*4+2] = (word >> 16) & 0xFF;
        output[i*4+3] = (word >> 24) & 0xFF;
    }
}

// CBC模式加密多块
void hw_aes_cbc_encrypt(const AESContext* ctx,
                        const uint8_t* input, uint8_t* output,
                        size_t length) {
    hw_aes_init(ctx);

    uint8_t iv[16];
    memcpy(iv, ctx->iv, 16);

    for (size_t i = 0; i < length; i += 16) {
        uint8_t block[16];

        // XOR with IV/前一块密文
        for (int j = 0; j < 16; j++) {
            block[j] = input[i + j] ^ iv[j];
        }

        hw_aes_encrypt_block(block, output + i);
        memcpy(iv, output + i, 16);
    }
}

// 性能对比: 硬件 vs 软件 AES
// 硬件: ~1000 MB/s (专用电路)
// 软件 (ARM Cortex-A53): ~50 MB/s (单核)
```

### 4. 侧信道防护

侧信道攻击利用功耗、电磁、时序等信息泄露。

```c
// 恒定时间比较 (防止时序攻击)
int constant_time_compare(const uint8_t* a, const uint8_t* b, size_t len) {
    uint8_t result = 0;
    for (size_t i = 0; i < len; i++) {
        result |= a[i] ^ b[i];  // 按位异或
    }
    return result;  // 0表示相等，非0表示不等
}

// 恒定时间查表 (AES S-box访问)
uint8_t constant_time_sbox_lookup(uint8_t index) {
    // 访问整个表，但只返回需要的值
    uint8_t result = 0;
    for (int i = 0; i < 256; i++) {
        uint8_t mask = -(i == index);  // 如果i==index，mask=0xFF，否则mask=0
        result |= aes_sbox[i] & mask;
    }
    return result;
}

// 功耗分析防护: 随机延迟
void random_delay(void) {
    volatile uint32_t delay = random_get() & 0xFF;
    for (volatile uint32_t i = 0; i < delay; i++);
}

// 掩码AES (抵御差分功耗分析 DPA)
typedef struct {
    uint8_t share1[16];
    uint8_t share2[16];
} MaskedState;

void masked_aes_encrypt(const MaskedState* input, MaskedState* output) {
    // 使用秘密共享技术，将中间状态分为两个份额
    // 每个份额单独处理，攻击者需要同时监控两个份额的功耗

    MaskedState state = *input;

    for (int round = 0; round < 10; round++) {
        // SubBytes (带掩码)
        for (int i = 0; i < 16; i++) {
            state.share1[i] = aes_sbox[state.share1[i]];
            state.share2[i] = aes_sbox[state.share2[i]];
        }

        // ShiftRows
        masked_shift_rows(&state);

        // MixColumns
        masked_mix_columns(&state);

        // AddRoundKey
        masked_add_round_key(&state, round_key[round]);
    }

    *output = state;
}
```

---

## 最佳实践

1. **纵深防御**: 多层安全机制，不依赖单一防护
2. **最小权限**: 安全世界代码尽量精简 (TCB最小化)
3. **安全审计**: 定期进行侧信道分析 (DPA/EMA)
4. **密钥管理**: 使用硬件安全模块 (HSM) 存储根密钥

---

## 权威参考

- [1] ARM TrustZone Technology, ARM Technical Reference
- [2] Trusted Computing Group, TPM 2.0 Library Specification
- [3] Intel SGX Explained, Victor Costan & Srinivas Devadas
- [4] "Side-Channel Analysis" by Stefan Mangard et al., Springer

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
> **内容深度**: L5 (研究级)
