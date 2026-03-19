/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\03_Measured_Boot.md
 * Line: 421
 * Language: c
 * Block ID: a7c18ceb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
