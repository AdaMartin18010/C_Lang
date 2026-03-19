/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\02_Secure_Boot_Chain.md
 * Line: 244
 * Language: c
 * Block ID: 352e6444
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// TPM PCR扩展
#include <tss2/tss2_esys.h>

// 测量启动上下文
typedef struct {
    ESYS_CONTEXT *tpm_ctx;
    TPMI_DH_PCR pcr_index;
} MeasureContext;

// 测量镜像并扩展PCR
int measure_image(MeasureContext *ctx, const uint8_t *image, size_t size,
                  const char *description) {
    // 计算镜像哈希
    uint8_t hash[SHA256_DIGEST_LENGTH];
    SHA256(image, size, hash);

    printf("Measuring: %s\n", description);
    printf("  Hash: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");

    // TPM PCR扩展
    TPM2B_DIGEST digest = {
        .size = SHA256_DIGEST_LENGTH,
    };
    memcpy(digest.buffer, hash, SHA256_DIGEST_LENGTH);

    ESYS_TR pcr_handle = ctx->pcr_index;

    TSS2_RC rc = Esys_PCR_Extend(ctx->tpm_ctx, pcr_handle,
                                  ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE,
                                  &digest);

    if (rc != TSS2_RC_SUCCESS) {
        printf("PCR Extend failed: 0x%x\n", rc);
        return -1;
    }

    // 记录到启动日志
    log_boot_event(description, hash, ctx->pcr_index);

    return 0;
}

// 测量整个启动链
void measured_boot_chain(void) {
    MeasureContext ctx = {
        .tpm_ctx = init_tpm(),
        .pcr_index = 10,  // 用于启动测量的PCR
    };

    // 测量Boot ROM (静态值，通常由TPM厂商提供)
    // ...

    // 测量BL1
    uint8_t *bl1 = load_image("bl1.bin");
    measure_image(&ctx, bl1, get_size("bl1.bin"), "BL1");

    // 测量BL2
    uint8_t *bl2 = load_image("bl2.bin");
    measure_image(&ctx, bl2, get_size("bl2.bin"), "BL2");

    // 测量设备树
    uint8_t *dtb = load_image(".dtb");
    measure_image(&ctx, dtb, get_size(".dtb"), "Device Tree");

    // 测量内核
    uint8_t *kernel = load_image("zImage");
    measure_image(&ctx, kernel, get_size("zImage"), "Kernel");

    // 生成启动日志摘要并扩展到另一个PCR
    finalize_boot_log(&ctx);
}
