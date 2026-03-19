/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\03_Measured_Boot.md
 * Line: 140
 * Language: c
 * Block ID: 2afd6492
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
