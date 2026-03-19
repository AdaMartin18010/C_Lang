/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\03_Measured_Boot.md
 * Line: 80
 * Language: c
 * Block ID: da10558b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
