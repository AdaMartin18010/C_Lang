/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\README.md
 * Line: 403
 * Language: c
 * Block ID: f30a38ac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
