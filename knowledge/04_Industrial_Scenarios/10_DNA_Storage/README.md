# DNA 存储


---

## 📑 目录

- [DNA 存储](#dna-存储)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [DNA 存储原理](#dna-存储原理)
    - [分子基础](#分子基础)
    - [编码流程](#编码流程)
  - [编解码算法](#编解码算法)
    - [1. 游程限制编码 (Run-Length Limited)](#1-游程限制编码-run-length-limited)
    - [2. 旋转编码（Rotation Encoding）](#2-旋转编码rotation-encoding)
    - [3. Huffman 编码压缩](#3-huffman-编码压缩)
  - [错误校正](#错误校正)
    - [1. Reed-Solomon 编码](#1-reed-solomon-编码)
    - [2. DNA 特定错误校正](#2-dna-特定错误校正)
    - [3. 地址索引与纠错](#3-地址索引与纠错)
  - [C 语言完整实现示例](#c-语言完整实现示例)
  - [性能优化](#性能优化)
    - [多线程并行处理](#多线程并行处理)
  - [参考资源](#参考资源)


---

## 概述

DNA 存储是一种利用脱氧核糖核酸（DNA）分子存储数字信息的技术。作为自然界最古老的信息存储介质，DNA 具有极高的存储密度（约 10^19 bytes/g）和极长的保存期限（可达数千年），被认为是未来冷数据存储的理想解决方案。

## DNA 存储原理

### 分子基础

DNA 由四种核苷酸组成：

- **A** (Adenine, 腺嘌呤)
- **T** (Thymine, 胸腺嘧啶)
- **C** (Cytosine, 胞嘧啶)
- **G** (Guanine, 鸟嘌呤)

信息编码映射：

```
00 → A
01 → T
10 → C
11 → G
```

### 编码流程

```c
// 二进制到 DNA 序列的基本编码
typedef enum {
    BASE_A = 0,
    BASE_T = 1,
    BASE_C = 2,
    BASE_G = 3
} DNABase;

// 2 bits 转 DNA 碱基
DNABase bits_to_base(uint8_t bits) {
    switch (bits & 0x03) {
        case 0: return BASE_A;
        case 1: return BASE_T;
        case 2: return BASE_C;
        case 3: return BASE_G;
    }
    return BASE_A;
}

// 将二进制数据编码为 DNA 序列
int encode_to_dna(const uint8_t *binary_data, size_t len,
                  char *dna_sequence, size_t dna_buf_size) {
    if (dna_buf_size < len * 4 + 1) {
        return -1;  // 缓冲区不足
    }

    const char base_chars[] = {'A', 'T', 'C', 'G'};
    size_t dna_idx = 0;

    for (size_t i = 0; i < len; i++) {
        uint8_t byte = binary_data[i];
        // 每个字节编码为 4 个碱基
        for (int j = 3; j >= 0; j--) {
            uint8_t bits = (byte >> (j * 2)) & 0x03;
            dna_sequence[dna_idx++] = base_chars[bits];
        }
    }
    dna_sequence[dna_idx] = '\0';
    return dna_idx;
}

// DNA 序列解码为二进制
int decode_from_dna(const char *dna_sequence, size_t dna_len,
                    uint8_t *binary_data, size_t bin_buf_size) {
    if (dna_len % 4 != 0 || bin_buf_size < dna_len / 4) {
        return -1;
    }

    size_t bin_idx = 0;
    for (size_t i = 0; i < dna_len; i += 4) {
        uint8_t byte = 0;
        for (int j = 0; j < 4; j++) {
            char base = dna_sequence[i + j];
            uint8_t bits;
            switch (base) {
                case 'A': bits = 0; break;
                case 'T': bits = 1; break;
                case 'C': bits = 2; break;
                case 'G': bits = 3; break;
                default: return -1;  // 无效碱基
            }
            byte = (byte << 2) | bits;
        }
        binary_data[bin_idx++] = byte;
    }
    return bin_idx;
}
```

## 编解码算法

### 1. 游程限制编码 (Run-Length Limited)

避免 DNA 合成和测序中的同聚物（homopolymer）问题：

```c
// RLL(2,∞) 编码 - 限制连续相同碱基数量
#define MAX_HOMOPOLYMER 2

typedef struct {
    char *output;
    size_t out_len;
    size_t capacity;
    char last_base;
    int run_length;
} RLLEncoder;

void rll_encode_init(RLLEncoder *enc, char *buffer, size_t cap) {
    enc->output = buffer;
    enc->capacity = cap;
    enc->out_len = 0;
    enc->last_base = '\0';
    enc->run_length = 0;
}

// 插入干扰碱基打破长游程
char get_alternative_base(char base, int alt_idx) {
    const char alternatives[] = {'A', 'T', 'C', 'G'};
    for (int i = 0; i < 4; i++) {
        if (alternatives[i] != base) {
            if (alt_idx-- == 0) return alternatives[i];
        }
    }
    return alternatives[0];
}

int rll_encode_byte(RLLEncoder *enc, DNABase base) {
    const char base_char = "ATCG"[base];

    if (base_char == enc->last_base) {
        enc->run_length++;
        if (enc->run_length >= MAX_HOMOPOLYMER) {
            // 插入干扰碱基
            char alt = get_alternative_base(base_char, 0);
            if (enc->out_len >= enc->capacity) return -1;
            enc->output[enc->out_len++] = alt;
            enc->run_length = 0;
        }
    } else {
        enc->last_base = base_char;
        enc->run_length = 1;
    }

    if (enc->out_len >= enc->capacity) return -1;
    enc->output[enc->out_len++] = base_char;
    return 0;
}
```

### 2. 旋转编码（Rotation Encoding）

平衡 GC 含量：

```c
// 旋转码确保 GC 含量在 50% 左右
void rotate_encode(const uint8_t *data, size_t len,
                   char *output, size_t out_cap) {
    const char *rotations[] = {
        "ACGT",  // 00 -> ACGT
        "CATG",  // 01 -> CATG
        "GTAC",  // 10 -> GTAC
        "TGCA"   // 11 -> TGCA
    };

    size_t out_idx = 0;
    int rotation_idx = 0;

    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];
        for (int j = 3; j >= 0; j--) {
            uint8_t bits = (byte >> (j * 2)) & 0x03;
            if (out_idx >= out_cap) return;
            output[out_idx++] = rotations[rotation_idx][bits];
            rotation_idx = (rotation_idx + 1) % 4;
        }
    }
    output[out_idx] = '\0';
}
```

### 3. Huffman 编码压缩

```c
// 基于 DNA 序列特性的 Huffman 编码
typedef struct HuffmanNode {
    char base;
    int freq;
    struct HuffmanNode *left, *right;
} HuffmanNode;

// 构建频率表
void build_freq_table(const char *dna_seq, size_t len,
                      int freq[256]) {
    memset(freq, 0, 256 * sizeof(int));
    for (size_t i = 0; i < len; i++) {
        freq[(unsigned char)dna_seq[i]]++;
    }
}

// 计算二聚体频率用于优化
void build_dimer_freq(const char *dna_seq, size_t len,
                      int dimer_freq[4][4]) {
    const char bases[] = "ATCG";
    memset(dimer_freq, 0, 16 * sizeof(int));

    for (size_t i = 0; i < len - 1; i++) {
        int b1 = -1, b2 = -1;
        for (int j = 0; j < 4; j++) {
            if (dna_seq[i] == bases[j]) b1 = j;
            if (dna_seq[i+1] == bases[j]) b2 = j;
        }
        if (b1 >= 0 && b2 >= 0) {
            dimer_freq[b1][b2]++;
        }
    }
}
```

## 错误校正

### 1. Reed-Solomon 编码

```c
// 简化的 Reed-Solomon 实现
#define RS_SYMBOL_SIZE 8
#define RS_N 255  // 码字长度
#define RS_K 223  // 数据符号数
#define RS_T 16   // 纠错能力

// Galois Field 乘法
uint8_t gf_mul(uint8_t a, uint8_t b, uint16_t poly) {
    uint8_t p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) p ^= a;
        int carry = a & 0x80;
        a <<= 1;
        if (carry) a ^= (poly & 0xFF);
        b >>= 1;
    }
    return p;
}

// RS 编码器
typedef struct {
    uint8_t generator[RS_T * 2 + 1];
    uint16_t poly;
} RSEncoder;

void rs_init(RSEncoder *rs, uint16_t primitive_poly) {
    rs->poly = primitive_poly;
    // 计算生成多项式
    rs->generator[0] = 1;
    for (int i = 0; i < RS_T * 2; i++) {
        rs->generator[i + 1] = 0;
    }
    // 简化实现：预计算生成多项式
}

// 计算校验符号
void rs_encode(RSEncoder *rs, const uint8_t *data, size_t len,
               uint8_t *parity) {
    memset(parity, 0, RS_T * 2);

    for (size_t i = 0; i < len; i++) {
        uint8_t feedback = data[i] ^ parity[0];
        for (int j = 0; j < RS_T * 2 - 1; j++) {
            parity[j] = parity[j + 1] ^
                        gf_mul(rs->generator[j + 1], feedback, rs->poly);
        }
        parity[RS_T * 2 - 1] = gf_mul(rs->generator[RS_T * 2],
                                      feedback, rs->poly);
    }
}
```

### 2. DNA 特定错误校正

```c
// 处理 DNA 存储特有错误（插入、删除、替换）
typedef enum {
    ERROR_NONE,
    ERROR_SUBSTITUTION,
    ERROR_INSERTION,
    ERROR_DELETION
} DNAErrorType;

// Levenshtein 距离计算用于纠错
int levenshtein_distance(const char *s1, const char *s2,
                         int len1, int len2) {
    int *prev = malloc((len2 + 1) * sizeof(int));
    int *curr = malloc((len2 + 1) * sizeof(int));

    for (int j = 0; j <= len2; j++) prev[j] = j;

    for (int i = 1; i <= len1; i++) {
        curr[0] = i;
        for (int j = 1; j <= len2; j++) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            int deletion = prev[j] + 1;
            int insertion = curr[j-1] + 1;
            int substitution = prev[j-1] + cost;

            curr[j] = deletion;
            if (insertion < curr[j]) curr[j] = insertion;
            if (substitution < curr[j]) curr[j] = substitution;
        }
        int *temp = prev;
        prev = curr;
        curr = temp;
    }

    int result = prev[len2];
    free(prev);
    free(curr);
    return result;
}
```

### 3. 地址索引与纠错

```c
// DNA 数据块地址和索引
typedef struct {
    uint32_t block_id;       // 块标识
    uint16_t payload_len;    // 有效载荷长度
    uint8_t rs_parity[32];   // RS 校验
    uint32_t crc32;          // CRC 校验
} DNABlockHeader;

#define DNA_BLOCK_SIZE 200  // DNA 寡核苷酸长度限制

int create_dna_block(uint32_t block_id, const uint8_t *payload,
                     size_t payload_len, char *dna_output,
                     RSEncoder *rs) {
    if (payload_len > DNA_BLOCK_SIZE - sizeof(DNABlockHeader) - 32) {
        return -1;
    }

    DNABlockHeader header = {
        .block_id = block_id,
        .payload_len = payload_len
    };

    // 组合头部和数据
    uint8_t block_data[DNA_BLOCK_SIZE];
    memcpy(block_data, &header, sizeof(header));
    memcpy(block_data + sizeof(header), payload, payload_len);

    // 计算 RS 校验
    rs_encode(rs, block_data, sizeof(header) + payload_len,
              header.rs_parity);

    // 计算 CRC
    header.crc32 = crc32(block_data, sizeof(header) + payload_len);

    // 更新块数据
    memcpy(block_data, &header, sizeof(header));

    // 编码为 DNA
    return encode_to_dna(block_data, sizeof(header) + payload_len + 32,
                         dna_output, DNA_BLOCK_SIZE * 4);
}
```

## C 语言完整实现示例

```c
// dna_storage.h
#ifndef DNA_STORAGE_H
#define DNA_STORAGE_H

#include <stdint.h>
#include <stddef.h>

// 编码配置
typedef struct {
    int use_rll;          // 启用游程限制
    int use_rotation;     // 启用旋转编码
    int gc_balance;       // GC 平衡
    int rs_t;             // RS 纠错能力
} DNAEncodeConfig;

// 编码器上下文
typedef struct {
    DNAEncodeConfig config;
    uint8_t *temp_buffer;
    size_t temp_size;
} DNAEncoder;

// API 函数
int dna_encoder_init(DNAEncoder *enc, const DNAEncodeConfig *cfg);
void dna_encoder_free(DNAEncoder *enc);
int dna_encode(DNAEncoder *enc, const uint8_t *data, size_t len,
               char **dna_output, size_t *dna_len);
int dna_decode(DNAEncoder *enc, const char *dna, size_t dna_len,
               uint8_t **data_output, size_t *data_len);
int dna_correct_errors(const char *dna_reads[], int num_reads,
                       char *corrected, size_t max_len);

#endif
```

```c
// dna_storage.c (核心实现)
#include "dna_storage.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

int dna_encoder_init(DNAEncoder *enc, const DNAEncodeConfig *cfg) {
    enc->config = *cfg;
    enc->temp_size = 1024 * 1024;  // 1MB 临时缓冲区
    enc->temp_buffer = malloc(enc->temp_size);
    return enc->temp_buffer ? 0 : -1;
}

void dna_encoder_free(DNAEncoder *enc) {
    free(enc->temp_buffer);
    enc->temp_buffer = NULL;
}

int dna_encode(DNAEncoder *enc, const uint8_t *data, size_t len,
               char **dna_output, size_t *dna_len) {
    // 分块处理
    size_t chunk_size = 100;  // 每块约 100 字节
    size_t num_chunks = (len + chunk_size - 1) / chunk_size;

    size_t dna_capacity = num_chunks * chunk_size * 8;
    *dna_output = malloc(dna_capacity);
    if (!*dna_output) return -1;

    size_t total_dna = 0;
    for (size_t i = 0; i < num_chunks; i++) {
        size_t offset = i * chunk_size;
        size_t this_chunk = (offset + chunk_size <= len) ?
                            chunk_size : (len - offset);

        // 添加地址前缀
        uint8_t chunk_with_addr[128];
        chunk_with_addr[0] = (i >> 8) & 0xFF;
        chunk_with_addr[1] = i & 0xFF;
        memcpy(chunk_with_addr + 2, data + offset, this_chunk);

        // RS 编码
        // ... 添加校验

        // 基础编码
        int dna_len = encode_to_dna(chunk_with_addr, this_chunk + 2 + 32,
                                    *dna_output + total_dna,
                                    dna_capacity - total_dna);
        if (dna_len < 0) {
            free(*dna_output);
            return -1;
        }

        // RLL 编码
        if (enc->config.use_rll) {
            // ... 应用 RLL
        }

        total_dna += dna_len;
        (*dna_output)[total_dna++] = '\n';  // 块分隔
    }

    *dna_len = total_dna;
    return 0;
}
```

## 性能优化

### 多线程并行处理

```c
#include <pthread.h>

#define NUM_THREADS 8

typedef struct {
    const uint8_t *data;
    size_t start;
    size_t end;
    char *output;
    DNAEncoder *encoder;
} EncodeTask;

void *encode_worker(void *arg) {
    EncodeTask *task = arg;
    size_t local_dna_len;

    // 独立编码每个块
    dna_encode(task->encoder, task->data + task->start,
               task->end - task->start, &task->output, &local_dna_len);

    return NULL;
}

int parallel_encode(const uint8_t *data, size_t len, char **output,
                    size_t *out_len) {
    pthread_t threads[NUM_THREADS];
    EncodeTask tasks[NUM_THREADS];

    size_t chunk_size = len / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        tasks[i].data = data;
        tasks[i].start = i * chunk_size;
        tasks[i].end = (i == NUM_THREADS - 1) ? len : (i + 1) * chunk_size;
        pthread_create(&threads[i], NULL, encode_worker, &tasks[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // 合并结果...
    return 0;
}
```

## 参考资源

- [DNA Data Storage Alliance](https://www.dnastoragealliance.org/)
- [Microsoft DNA Storage Research](https://www.microsoft.com/en-us/research/project/dna-storage/)
- [Goldman et al. - Nature 2013](https://www.nature.com/)
- [Erlich & Zielinski - Science 2017](https://www.science.org/)
