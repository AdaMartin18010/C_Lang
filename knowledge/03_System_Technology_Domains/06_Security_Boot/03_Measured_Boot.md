# 度量启动与TPM实现


---

## 📑 目录

- [度量启动与TPM实现](#度量启动与tpm实现)
  - [📑 目录](#-目录)
  - [1. 度量启动原理（Trusted Boot）](#1-度量启动原理trusted-boot)
    - [1.1 什么是度量启动](#11-什么是度量启动)
    - [1.2 度量启动流程](#12-度量启动流程)
    - [1.3 PCR寄存器分配](#13-pcr寄存器分配)
  - [2. TPM PCR寄存器使用](#2-tpm-pcr寄存器使用)
    - [2.1 PCR扩展操作](#21-pcr扩展操作)
    - [2.2 TSS2库基础](#22-tss2库基础)
  - [3. TPM2 C语言实现](#3-tpm2-c语言实现)
    - [3.1 核心实现代码](#31-核心实现代码)
    - [3.2 事件日志解析](#32-事件日志解析)
    - [3.3 启动度量验证](#33-启动度量验证)
  - [4. 完整的度量启动示例](#4-完整的度量启动示例)
  - [5. 编译与运行](#5-编译与运行)
  - [6. 安全注意事项](#6-安全注意事项)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 1. 度量启动原理（Trusted Boot）

### 1.1 什么是度量启动

度量启动（Measured Boot）是一种安全启动技术，通过在系统启动过程中对关键组件进行度量（计算哈希值）并记录到可信平台模块（TPM）中，确保系统启动链的完整性。
与Secure Boot直接阻止未签名代码执行不同，度量启动允许执行但会记录执行历史，供后续远程证明（Remote Attestation）验证。

### 1.2 度量启动流程

```text
CRT (Core Root of Trust) 核心信任根
    ↓ 度量 BIOS/UEFI Firmware
TPM PCR[0] ← Hash(Firmware)
    ↓ 度量 Bootloader
TPM PCR[4] ← Hash(Bootloader)
    ↓ 度量 OS Kernel
TPM PCR[8] ← Hash(Kernel)
    ↓ 度量 关键系统组件
TPM PCR[9+] ← Hash(Components)
```

### 1.3 PCR寄存器分配

| PCR索引 | 标准用途 | 说明 |
|---------|----------|------|
| 0 | SRTM, BIOS | 静态信任根度量 |
| 1 | BIOS配置 | 平台配置 |
| 2 | Option ROM | 扩展固件 |
| 3 | Option ROM配置 | 扩展配置 |
| 4 | MBR, Bootloader | 主引导记录 |
| 5 | Bootloader配置 | 启动配置 |
| 6 | State Transition | 状态转换 |
| 7 | Secure Boot | 安全启动策略 |
| 8-15 | OS控制 | 操作系统使用 |
| 16 | Debug | 调试保留 |
| 17-22 | DRTM/TXT | 动态信任根 |
| 23 | 应用专用 | 自定义应用 |

## 2. TPM PCR寄存器使用

### 2.1 PCR扩展操作

PCR（Platform Configuration Register）是TPM中的特殊寄存器，其值只能通过"扩展"操作修改：

```text
NewPCR = Hash(OldPCR || NewMeasurement)
```

这种链式结构确保任何篡改都会被检测到。

### 2.2 TSS2库基础

```c
// tpm_measure.h
#ifndef TPM_MEASURE_H
#define TPM_MEASURE_H

#include <tss2/tss2_esys.h>
#include <tss2/tss2_rc.h>
#include <stdint.h>
#include <stdbool.h>

#define TPM_PCR_COUNT 24
#define SHA256_DIGEST_SIZE 32
#define TPM_EVENT_LOG_MAX 4096

// TPM上下文
typedef struct {
    ESYS_CONTEXT *esys_ctx;
    TSS2_TCTI_CONTEXT *tcti_ctx;
    bool initialized;
} TPMContext;

// 度量事件结构（TCG规范）
typedef struct {
    uint32_t pcr_index;
    uint32_t event_type;
    uint8_t digest[SHA256_DIGEST_SIZE];
    uint32_t event_data_size;
    uint8_t event_data[];
} TCGEvent;

// 初始化/清理TPM连接
int tpm_init(TPMContext *ctx);
void tpm_cleanup(TPMContext *ctx);

// PCR操作
int tpm_pcr_extend(TPMContext *ctx, uint32_t pcr_index,
                   const uint8_t *measurement, size_t len);
int tpm_pcr_read(TPMContext *ctx, uint32_t pcr_index,
                 uint8_t *digest, size_t *digest_len);
int tpm_pcr_reset(TPMContext *ctx, uint32_t pcr_index);

// 批量读取多个PCR
int tpm_pcr_read_all(TPMContext *ctx, uint8_t pcr_values[TPM_PCR_COUNT][SHA256_DIGEST_SIZE]);

// 度量文件
int tpm_measure_file(TPMContext *ctx, uint32_t pcr_index, const char *filepath);
int tpm_measure_buffer(TPMContext *ctx, uint32_t pcr_index,
                       const uint8_t *data, size_t len, const char *description);

// 事件日志
int tpm_read_event_log(uint8_t *log_buffer, size_t *log_size);
void tpm_dump_event_log(const uint8_t *log_buffer, size_t log_size);

#endif
```

## 3. TPM2 C语言实现

### 3.1 核心实现代码

```c
// tpm_measure.c
#include "tpm_measure.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

// 计算SHA256哈希
static int calculate_sha256(const uint8_t *data, size_t len, uint8_t *hash) {
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) return -1;

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1 ||
        EVP_DigestUpdate(ctx, data, len) != 1 ||
        EVP_DigestFinal_ex(ctx, hash, NULL) != 1) {
        EVP_MD_CTX_free(ctx);
        return -1;
    }

    EVP_MD_CTX_free(ctx);
    return 0;
}

// 初始化TPM连接
int tpm_init(TPMContext *ctx) {
    TSS2_RC rc;

    memset(ctx, 0, sizeof(TPMContext));

    // 使用默认TCTI（通常是设备TCTI /dev/tpm0 或 /dev/tpmrm0）
    rc = Tss2_TctiLdr_Initialize(NULL, &ctx->tcti_ctx);
    if (rc != TSS2_RC_SUCCESS) {
        fprintf(stderr, "Failed to initialize TCTI: 0x%x\n", rc);
        return -1;
    }

    // 创建ESAPI上下文
    rc = Esys_Initialize(&ctx->esys_ctx, ctx->tcti_ctx, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        fprintf(stderr, "Failed to initialize ESYS: 0x%x\n", rc);
        Tss2_TctiLdr_Finalize(&ctx->tcti_ctx);
        return -1;
    }

    // 启动会话
    ESYS_TR session;
    TPMA_SESSION session_attrs = {0};
    rc = Esys_StartAuthSession(ctx->esys_ctx, ESYS_TR_NONE, ESYS_TR_NONE,
                               ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE,
                               NULL, TPM2_SE_HMAC, &symmetric, TPM2_ALG_SHA256,
                               &session);
    if (rc == TSS2_RC_SUCCESS) {
        Esys_TR_SetAuth(ctx->esys_ctx, ESYS_TR_RH_PLATFORM, &null_auth);
    }

    ctx->initialized = true;
    printf("TPM initialized successfully\n");
    return 0;
}

void tpm_cleanup(TPMContext *ctx) {
    if (ctx->esys_ctx) {
        Esys_Finalize(&ctx->esys_ctx);
    }
    if (ctx->tcti_ctx) {
        Tss2_TctiLdr_Finalize(&ctx->tcti_ctx);
    }
    ctx->initialized = false;
}

// 扩展PCR值
int tpm_pcr_extend(TPMContext *ctx, uint32_t pcr_index,
                   const uint8_t *measurement, size_t len) {
    TSS2_RC rc;
    TPML_DIGEST_VALUES digests = {0};

    if (!ctx->initialized) {
        fprintf(stderr, "TPM not initialized\n");
        return -1;
    }

    if (pcr_index >= TPM_PCR_COUNT) {
        fprintf(stderr, "Invalid PCR index: %u\n", pcr_index);
        return -1;
    }

    // 计算输入数据的SHA256哈希
    uint8_t hash[SHA256_DIGEST_SIZE];
    if (calculate_sha256(measurement, len, hash) != 0) {
        fprintf(stderr, "Failed to calculate hash\n");
        return -1;
    }

    // 准备摘要值
    digests.count = 1;
    digests.digests[0].hashAlg = TPM2_ALG_SHA256;
    memcpy(digests.digests[0].digest.sha256, hash, SHA256_DIGEST_SIZE);

    // 执行PCR扩展
    rc = Esys_PCR_Extend(ctx->esys_ctx, pcr_index,
                         ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE,
                         &digests);

    if (rc != TSS2_RC_SUCCESS) {
        fprintf(stderr, "PCR_Extend failed: 0x%x\n", rc);
        return -1;
    }

    printf("Extended PCR[%u] with measurement: ", pcr_index);
    for (int i = 0; i < SHA256_DIGEST_SIZE; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");

    return 0;
}

// 读取PCR值
int tpm_pcr_read(TPMContext *ctx, uint32_t pcr_index,
                 uint8_t *digest, size_t *digest_len) {
    TSS2_RC rc;
    TPML_SELECTION pcr_selection_in = {0};
    TPML_DIGEST *pcr_values = NULL;

    if (!ctx->initialized) {
        fprintf(stderr, "TPM not initialized\n");
        return -1;
    }

    // 配置PCR选择
    pcr_selection_in.count = 1;
    pcr_selection_in.pcrSelections[0].hash = TPM2_ALG_SHA256;
    pcr_selection_in.pcrSelections[0].sizeofSelect = 3;
    pcr_selection_in.pcrSelections[0].pcrSelect[pcr_index / 8] =
        1 << (pcr_index % 8);

    // 读取PCR
    rc = Esys_PCR_Read(ctx->esys_ctx, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE,
                       &pcr_selection_in, NULL, NULL, &pcr_values);

    if (rc != TSS2_RC_SUCCESS) {
        fprintf(stderr, "PCR_Read failed: 0x%x\n", rc);
        return -1;
    }

    if (pcr_values->count > 0) {
        memcpy(digest, pcr_values->digests[0].buffer,
               pcr_values->digests[0].size);
        *digest_len = pcr_values->digests[0].size;
    }

    free(pcr_values);
    return 0;
}

// 批量读取所有PCR
int tpm_pcr_read_all(TPMContext *ctx,
                     uint8_t pcr_values[TPM_PCR_COUNT][SHA256_DIGEST_SIZE]) {
    for (uint32_t i = 0; i < TPM_PCR_COUNT; i++) {
        size_t len = SHA256_DIGEST_SIZE;
        if (tpm_pcr_read(ctx, i, pcr_values[i], &len) != 0) {
            memset(pcr_values[i], 0, SHA256_DIGEST_SIZE);
        }
    }
    return 0;
}

// 度量文件内容
int tpm_measure_file(TPMContext *ctx, uint32_t pcr_index, const char *filepath) {
    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        perror("Failed to open file");
        return -1;
    }

    // 获取文件大小
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // 读取文件内容
    uint8_t *buffer = malloc(size);
    if (!buffer) {
        fclose(fp);
        return -1;
    }

    if (fread(buffer, 1, size, fp) != (size_t)size) {
        free(buffer);
        fclose(fp);
        return -1;
    }
    fclose(fp);

    // 执行度量
    int ret = tpm_measure_buffer(ctx, pcr_index, buffer, size, filepath);

    free(buffer);
    return ret;
}

// 度量数据缓冲区
int tpm_measure_buffer(TPMContext *ctx, uint32_t pcr_index,
                       const uint8_t *data, size_t len, const char *description) {
    printf("Measuring '%s' (%zu bytes) into PCR[%u]...\n",
           description ? description : "buffer", len, pcr_index);

    return tpm_pcr_extend(ctx, pcr_index, data, len);
}
```

### 3.2 事件日志解析

```c
// 读取TCG事件日志（Linux特定）
int tpm_read_event_log(uint8_t *log_buffer, size_t *log_size) {
    const char *log_path = "/sys/kernel/security/tpm0/binary_bios_measurements";

    FILE *fp = fopen(log_path, "rb");
    if (!fp) {
        // 尝试替代路径
        log_path = "/sys/kernel/security/integrity/ima/binary_runtime_measurements";
        fp = fopen(log_path, "rb");
        if (!fp) {
            perror("Failed to open event log");
            return -1;
        }
    }

    size_t read = fread(log_buffer, 1, TPM_EVENT_LOG_MAX, fp);
    fclose(fp);

    *log_size = read;
    return 0;
}

// 解析并显示事件日志
void tpm_dump_event_log(const uint8_t *log_buffer, size_t log_size) {
    printf("\n=== TCG Event Log ===\n");
    printf("Total size: %zu bytes\n\n", log_size);

    size_t offset = 0;
    int event_count = 0;

    while (offset < log_size) {
        // 解析TCG PCClient事件（简化版本）
        if (log_size - offset < sizeof(TCGEvent)) {
            break;
        }

        TCGEvent *event = (TCGEvent *)(log_buffer + offset);

        printf("Event %d:\n", event_count++);
        printf("  PCR Index: %u\n", event->pcr_index);
        printf("  Event Type: 0x%08x\n", event->event_type);
        printf("  Digest: ");
        for (int i = 0; i < SHA256_DIGEST_SIZE; i++) {
            printf("%02x", event->digest[i]);
        }
        printf("\n");

        if (event->event_data_size > 0 && event->event_data_size < 256) {
            printf("  Event Data (%u bytes): ", event->event_data_size);
            for (uint32_t i = 0; i < event->event_data_size && i < 32; i++) {
                printf("%02x ", event->event_data[i]);
            }
            printf("...\n");
        }
        printf("\n");

        offset += sizeof(TCGEvent) + event->event_data_size;
    }

    printf("Total events: %d\n", event_count);
}
```

### 3.3 启动度量验证

```c
// boot_verify.h
#ifndef BOOT_VERIFY_H
#define BOOT_VERIFY_H

#include "tpm_measure.h"
#include <stdbool.h>

// 黄金参考值（来自可信源）
typedef struct {
    uint32_t pcr_index;
    uint8_t expected_hash[SHA256_DIGEST_SIZE];
    const char *component_name;
} GoldenReference;

// 启动验证上下文
typedef struct {
    GoldenReference *references;
    int reference_count;
    bool allow_unknown_components;
} VerifyContext;

// 验证函数
VerifyContext* verify_context_new(void);
void verify_context_free(VerifyContext *ctx);
void verify_add_reference(VerifyContext *ctx, uint32_t pcr_index,
                          const uint8_t *hash, const char *name);

bool verify_boot_integrity(VerifyContext *ctx, TPMContext *tpm);
bool verify_pcr_value(VerifyContext *ctx, uint32_t pcr_index,
                      const uint8_t *actual_hash);

// 生成引证（Attestation Quote）
int generate_attestation_quote(TPMContext *ctx, uint32_t *pcr_list, int pcr_count,
                               uint8_t *quote_data, size_t *quote_len,
                               uint8_t *signature, size_t *sig_len);

#endif
```

```c
// boot_verify.c
#include "boot_verify.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

VerifyContext* verify_context_new(void) {
    VerifyContext *ctx = calloc(1, sizeof(VerifyContext));
    ctx->references = calloc(32, sizeof(GoldenReference));
    ctx->reference_count = 0;
    ctx->allow_unknown_components = false;
    return ctx;
}

void verify_context_free(VerifyContext *ctx) {
    if (ctx) {
        free(ctx->references);
        free(ctx);
    }
}

void verify_add_reference(VerifyContext *ctx, uint32_t pcr_index,
                          const uint8_t *hash, const char *name) {
    if (ctx->reference_count >= 32) return;

    GoldenReference *ref = &ctx->references[ctx->reference_count++];
    ref->pcr_index = pcr_index;
    memcpy(ref->expected_hash, hash, SHA256_DIGEST_SIZE);
    ref->component_name = name;
}

bool verify_pcr_value(VerifyContext *ctx, uint32_t pcr_index,
                      const uint8_t *actual_hash) {
    for (int i = 0; i < ctx->reference_count; i++) {
        if (ctx->references[i].pcr_index == pcr_index) {
            if (memcmp(ctx->references[i].expected_hash,
                       actual_hash, SHA256_DIGEST_SIZE) == 0) {
                printf("✓ PCR[%u] '%s' verified OK\n",
                       pcr_index, ctx->references[i].component_name);
                return true;
            } else {
                printf("✗ PCR[%u] '%s' MISMATCH!\n",
                       pcr_index, ctx->references[i].component_name);
                printf("  Expected: ");
                for (int j = 0; j < SHA256_DIGEST_SIZE; j++) {
                    printf("%02x", ctx->references[i].expected_hash[j]);
                }
                printf("\n  Actual:   ");
                for (int j = 0; j < SHA256_DIGEST_SIZE; j++) {
                    printf("%02x", actual_hash[j]);
                }
                printf("\n");
                return false;
            }
        }
    }

    if (!ctx->allow_unknown_components) {
        printf("? PCR[%u] has no reference value\n", pcr_index);
        return false;
    }
    return true;
}

bool verify_boot_integrity(VerifyContext *ctx, TPMContext *tpm) {
    uint8_t pcr_values[TPM_PCR_COUNT][SHA256_DIGEST_SIZE];
    bool all_verified = true;

    printf("\n=== Boot Integrity Verification ===\n\n");

    // 读取所有PCR值
    if (tpm_pcr_read_all(tpm, pcr_values) != 0) {
        fprintf(stderr, "Failed to read PCR values\n");
        return false;
    }

    // 验证已知的PCR
    for (int i = 0; i < ctx->reference_count; i++) {
        if (!verify_pcr_value(ctx, ctx->references[i].pcr_index,
                              pcr_values[ctx->references[i].pcr_index])) {
            all_verified = false;
        }
    }

    printf("\n=== Verification Result: %s ===\n",
           all_verified ? "PASS" : "FAIL");

    return all_verified;
}
```

## 4. 完整的度量启动示例

```c
// measured_boot_demo.c
#include "tpm_measure.h"
#include "boot_verify.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 模拟系统组件
static const char *BOOT_COMPONENTS[] = {
    "uefi_firmware",
    "bootloader",
    "kernel",
    "initramfs",
    "kernel_modules",
    "systemd",
    "security_policy"
};

static const uint32_t BOOT_PCR_MAP[] = {0, 4, 8, 9, 9, 10, 11};

// 模拟启动过程度量
void simulate_boot_measurement(TPMContext *tpm) {
    printf("\n=== Simulating Boot Measurement Process ===\n\n");

    for (size_t i = 0; i < sizeof(BOOT_COMPONENTS)/sizeof(char*); i++) {
        // 生成组件的"度量值"（实际应为代码哈希）
        char component_data[256];
        snprintf(component_data, sizeof(component_data),
                 "Component: %s v1.0.0", BOOT_COMPONENTS[i]);

        // 扩展到对应PCR
        tpm_measure_buffer(tpm, BOOT_PCR_MAP[i],
                           (uint8_t*)component_data, strlen(component_data),
                           BOOT_COMPONENTS[i]);

        // 模拟启动延迟
        usleep(100000);
    }
}

// 显示所有PCR值
void display_all_pcrs(TPMContext *tpm) {
    uint8_t pcr_values[TPM_PCR_COUNT][SHA256_DIGEST_SIZE];

    printf("\n=== Current PCR Values ===\n\n");

    if (tpm_pcr_read_all(tpm, pcr_values) != 0) {
        fprintf(stderr, "Failed to read PCR values\n");
        return;
    }

    for (int i = 0; i < TPM_PCR_COUNT; i++) {
        bool is_zero = true;
        for (int j = 0; j < SHA256_DIGEST_SIZE; j++) {
            if (pcr_values[i][j] != 0) {
                is_zero = false;
                break;
            }
        }

        if (!is_zero) {
            printf("PCR[%02d]: ", i);
            for (int j = 0; j < SHA256_DIGEST_SIZE; j++) {
                printf("%02x", pcr_values[i][j]);
            }
            printf("\n");
        }
    }
}

int main(int argc, char *argv[]) {
    TPMContext tpm;

    printf("=== Measured Boot Demo ===\n");
    printf("Using TPM2 TSS2-ESAPI\n\n");

    // 初始化TPM
    if (tpm_init(&tpm) != 0) {
        fprintf(stderr, "TPM initialization failed. Using simulation mode.\n");
        // 继续演示，但使用模拟数据
    }

    // 模拟启动度量
    simulate_boot_measurement(&tpm);

    // 显示PCR值
    display_all_pcrs(&tpm);

    // 读取事件日志
    uint8_t event_log[TPM_EVENT_LOG_MAX];
    size_t log_size;
    if (tpm_read_event_log(event_log, &log_size) == 0) {
        tpm_dump_event_log(event_log, log_size);
    }

    // 创建验证上下文并验证
    VerifyContext *verify = verify_context_new();

    // 添加一些模拟的黄金参考值
    uint8_t dummy_hash[SHA256_DIGEST_SIZE];
    memset(dummy_hash, 0xAB, SHA256_DIGEST_SIZE);
    verify_add_reference(verify, 0, dummy_hash, "UEFI Firmware");

    verify_boot_integrity(verify, &tpm);

    // 清理
    verify_context_free(verify);
    tpm_cleanup(&tpm);

    return 0;
}

// 编译说明:
// gcc -o measured_boot measured_boot_demo.c tpm_measure.c boot_verify.c \
//     -ltss2-esys -ltss2-tctildr -lcrypto -lssl
```

## 5. 编译与运行

```bash
# 安装依赖（Ubuntu/Debian）
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
sudo apt-get install libtss2-dev libssl-dev

# 编译
gcc -o measured_boot \
    measured_boot_demo.c \
    tpm_measure.c \
    boot_verify.c \
    -ltss2-esys -ltss2-tctildr -ltss2-mu -ltss2-sys \
    -lcrypto -lssl -O2 -std=c17

# 运行（需要root权限访问TPM）
sudo ./measured_boot
```

## 6. 安全注意事项

1. **物理安全**：TPM芯片可能被物理攻击，高安全场景需使用TPM 2.0的防篡改功能
2. **密钥管理**：AK（Attestation Key）需要安全存储和使用
3. **PCR重置**：某些PCR（如16-23）可以在重启时重置，应用PCR需选择合适的索引
4. **隐私保护**：PCR值可能泄露系统配置信息，远程证明时注意隐私


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
