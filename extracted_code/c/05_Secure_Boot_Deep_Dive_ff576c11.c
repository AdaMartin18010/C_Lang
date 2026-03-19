/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 1289
 * Language: c
 * Block ID: ff576c11
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * TPM 2.0 PCR扩展实现
 * 用于测量启动过程中的组件
 */

#include <tss2/tss2_esys.h>

/* PCR索引定义 */
#define PCR_PLATFORM_CONFIG     0
#define PCR_CORE_CODE           1
#define PCR_CORE_DATA           2
#define PCR_BOOT_LOADER         3
#define PCR_BOOT_LOADER_CONFIG  4
#define PCR_BOOT_LOADER_CODE    5
#define PCR_SECURE_BOOT_POLICY  6
#define PCR_KERNEL_COMMAND_LINE 8
#define PCR_KERNEL_IMAGE        9
#define PCR_INITRD              10

/* TPM上下文 */
typedef struct {
    ESYS_CONTEXT *ctx;
    ESYS_TR session;
    TPM2B_DIGEST last_event;
} tpm_measured_boot_t;

static tpm_measured_boot_t g_tpm;

/* 初始化测量启动 */
int measured_boot_init(void) {
    TSS2_RC rc;

    /* 初始化ESAPI */
    rc = Esys_Initialize(&g_tpm.ctx, NULL, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        ERROR("Failed to initialize TPM: 0x%x\n", rc);
        return -1;
    }

    /* 启动TPM */
    TPM2B_STARTUP_CLEAR startup = {0};
    rc = Esys_Startup(g_tpm.ctx, TPM2_SU_CLEAR);
    if (rc != TPM2_RC_SUCCESS && rc != TPM2_RC_INITIALIZE) {
        ERROR("TPM startup failed: 0x%x\n", rc);
        return -1;
    }

    /* 读取初始PCR值 */
    INFO("TPM Initialized for measured boot\n");
    return 0;
}

/* 扩展PCR */
int extend_pcr(uint32_t pcr_index, const uint8_t *measurement, uint32_t len) {
    TSS2_RC rc;
    TPM2B_DIGEST digest = {.size = len};
    memcpy(digest.buffer, measurement, len);

    /* 创建PCR选择 */
    TPML_PCR_SELECTION pcr_selection = {
        .count = 1,
        .pcrSelections[0] = {
            .hash = TPM2_ALG_SHA256,
            .sizeofSelect = 3,
            .pcrSelect = {0}
        }
    };
    pcr_selection.pcrSelections[0].pcrSelect[pcr_index / 8] =
        1 << (pcr_index % 8);

    /* 执行扩展 */
    rc = Esys_PCR_Extend(g_tpm.ctx, pcr_index,
                         ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE,
                         &digest);
    if (rc != TSS2_RC_SUCCESS) {
        ERROR("PCR extend failed: 0x%x\n", rc);
        return -1;
    }

    /* 记录事件 */
    tpm_event_log_add(pcr_index, measurement, len, "Component measured");

    INFO("PCR[%u] extended with measurement\n", pcr_index);
    return 0;
}

/* 批量扩展（用于启动组件链） */
int extend_boot_component(const char *name,
                          const void *data, size_t len,
                          uint32_t pcr_index) {
    uint8_t hash[SHA256_DIGEST_LENGTH];

    /* 计算组件哈希 */
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, data, len);
    SHA256_Final(hash, &ctx);

    INFO("Measuring %s: ", name);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        INFO("%02x", hash[i]);
    }
    INFO("\n");

    /* 扩展PCR */
    return extend_pcr(pcr_index, hash, SHA256_DIGEST_LENGTH);
}

/* 读取PCR值 */
int read_pcr(uint32_t pcr_index, uint8_t *value, size_t *len) {
    TSS2_RC rc;

    TPML_PCR_SELECTION pcr_selection = {
        .count = 1,
        .pcrSelections[0] = {
            .hash = TPM2_ALG_SHA256,
            .sizeofSelect = 3,
            .pcrSelect = {0}
        }
    };
    pcr_selection.pcrSelections[0].pcrSelect[pcr_index / 8] =
        1 << (pcr_index % 8);

    UINT32 pcr_update_counter;
    TPML_PCR_SELECTION *pcr_selection_out = NULL;
    TPML_DIGEST *pcr_values = NULL;

    rc = Esys_PCR_Read(g_tpm.ctx,
                       ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE,
                       &pcr_selection,
                       &pcr_update_counter,
                       &pcr_selection_out,
                       &pcr_values);

    if (rc != TSS2_RC_SUCCESS) {
        ERROR("PCR read failed: 0x%x\n", rc);
        return -1;
    }

    if (pcr_values->count > 0) {
        *len = pcr_values->digests[0].size;
        memcpy(value, pcr_values->digests[0].buffer, *len);
    }

    Esys_Free(pcr_selection_out);
    Esys_Free(pcr_values);

    return 0;
}
