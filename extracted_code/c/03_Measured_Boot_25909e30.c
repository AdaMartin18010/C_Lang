/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\03_Measured_Boot.md
 * Line: 461
 * Language: c
 * Block ID: 25909e30
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
