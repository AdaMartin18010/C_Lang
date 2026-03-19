/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 841
 * Language: c
 * Block ID: 91ffe2c4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 完整的安全启动流程实现
 * 涵盖从ROM到内核的完整链条
 */

#include <crypto/rsa.h>
#include <crypto/sha256.h>
#include <tpm/tpm2.h>

/* 根公钥（烧录在OTP/eFuse中） */
static const uint8_t root_public_key[520] = {
    /* RSA-4096公钥 - 实际应从硬件读取 */
    [0 ... 519] = 0x00
};

/* 启动阶段定义 */
typedef enum {
    STAGE_ROM = 0,
    STAGE_BL1,
    STAGE_BL2,
    STAGE_BL31,
    STAGE_BL32,
    STAGE_BL33,
    STAGE_KERNEL,
    STAGE_MAX,
} boot_stage_t;

/* PCR分配 */
#define PCR_BOOT_LOADER     0   /* BL1-BL2 */
#define PCR_BOOT_CONFIG     1   /* 启动配置 */
#define PCR_PLATFORM        2   /* 平台代码 */
#define PCR_BL31            3   /* EL3运行时 */
#define PCR_BL32            4   /* TEE */
#define PCR_BL33            5   /* Bootloader */
#define PCR_KERNEL          6   /* 操作系统内核 */
#define PCR_KERNEL_CMD      7   /* 内核命令行 */

/* 安全启动上下文 */
typedef struct {
    boot_stage_t current_stage;
    uint32_t security_version[STAGE_MAX];
    bool measured_boot;
    bool secure_boot;
    tpm_context_t *tpm;
} secure_boot_ctx_t;

static secure_boot_ctx_t g_boot_ctx;

/* 初始化安全启动 */
void secure_boot_init(void) {
    memset(&g_boot_ctx, 0, sizeof(g_boot_ctx));

    /* 读取启动配置 */
    g_boot_ctx.secure_boot = is_secure_boot_fused();
    g_boot_ctx.measured_boot = is_measured_boot_fused();

    /* 初始化TPM */
    if (g_boot_ctx.measured_boot) {
        g_boot_ctx.tpm = tpm_init(TPM2_INTERFACE_FIFO);
        if (!g_boot_ctx.tpm) {
            /* TPM初始化失败，根据策略决定 */
            if (is_tpm_required()) {
                panic("TPM required but not available");
            }
        }
    }

    INFO("Secure Boot: %s\n", g_boot_ctx.secure_boot ? "Enabled" : "Disabled");
    INFO("Measured Boot: %s\n", g_boot_ctx.measured_boot ? "Enabled" : "Disabled");
}

/* 验证并加载下一阶段 */
int secure_boot_load_next(boot_stage_t next_stage,
                          const void *image_data,
                          size_t image_size,
                          const image_metadata_t *metadata,
                          void **entry_point) {

    uint8_t computed_hash[64];
    uint8_t signature[512];

    INFO("Loading stage %d...\n", next_stage);

    /* 1. 计算镜像哈希 */
    sha256(image_data, image_size, computed_hash);

    /* 2. 测量启动：扩展PCR */
    if (g_boot_ctx.measured_boot && g_boot_ctx.tpm) {
        uint32_t pcr_index = stage_to_pcr(next_stage);
        tpm_pcr_extend(g_boot_ctx.tpm, pcr_index,
                       TPM_ALG_SHA256, computed_hash, 32);

        /* 记录事件日志 */
        tpm_event_log_add(next_stage, computed_hash,
                         metadata->version, metadata->description);
    }

    /* 3. 安全启动：验证签名 */
    if (g_boot_ctx.secure_boot) {
        /* 验证安全版本（反滚动） */
        if (metadata->security_version < g_boot_ctx.security_version[next_stage]) {
            ERROR("Rollback detected! Current: %u, New: %u\n",
                  g_boot_ctx.security_version[next_stage],
                  metadata->security_version);
            return ERR_ROLLBACK_DETECTED;
        }

        /* 验证哈希匹配 */
        if (memcmp(computed_hash, metadata->image_hash, 32) != 0) {
            ERROR("Image hash mismatch!\n");
            return ERR_HASH_VERIFY_FAIL;
        }

        /* 验证签名 */
        const uint8_t *pubkey = get_stage_pubkey(next_stage);
        if (!pubkey) {
            ERROR("No public key for stage %d\n", next_stage);
            return ERR_NO_PUBKEY;
        }

        int ret = rsa_verify_pss(pubkey, metadata->signature_alg,
                                 computed_hash, 32,
                                 metadata->signature, metadata->sig_len);
        if (ret != 0) {
            ERROR("Signature verification failed!\n");
            return ERR_SIGNATURE_INVALID;
        }

        /* 更新安全版本 */
        g_boot_ctx.security_version[next_stage] = metadata->security_version;

        INFO("Stage %d verified OK (SVN: %u)\n",
             next_stage, metadata->security_version);
    }

    /* 4. 复制到执行地址 */
    void *exec_addr = (void *)metadata->load_address;
    memcpy(exec_addr, image_data, image_size);

    /* 5. 刷新指令缓存 */
    flush_icache_range(exec_addr, image_size);

    *entry_point = (void *)metadata->entry_point;
    return 0;
}

/* 主启动流程 */
void secure_boot_flow(void) {
    secure_boot_init();
    void *entry;
    int ret;

    /* Stage 1: 加载BL2 */
    {
        fip_image_t bl2_img;
        ret = fip_load_image(FIP_BL2, &bl2_img);
        if (ret) panic("Failed to load BL2");

        ret = secure_boot_load_next(STAGE_BL2,
                                     bl2_img.data, bl2_img.size,
                                     &bl2_img.metadata, &entry);
        if (ret) {
            log_security_event(EVENT_BL2_AUTH_FAIL, ret);
            if (g_boot_ctx.secure_boot) panic("BL2 auth failed");
        }

        bl2_entry_t bl2_entry = (bl2_entry_t)entry;
        bl2_entry(&g_boot_ctx);
    }

    /* 后续阶段在BL2中继续... */
}
