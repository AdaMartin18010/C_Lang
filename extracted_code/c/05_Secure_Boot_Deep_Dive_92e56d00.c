/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 535
 * Language: c
 * Block ID: 92e56d00
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * BL31: EL3 Runtime Firmware
 * 负责安全监控调用处理和系统电源管理
 */

#include <arch_helpers.h>
#include <context_mgmt.h>

/* SMC函数ID定义 */
#define SMC_FNID_POWER_STATE    0xC2000001
#define SMC_FNID_SYSTEM_OFF     0xC2000002
#define SMC_FNID_SYSTEM_RESET   0xC2000003
#define SMC_FNID_TPM_OP         0xC2000100
#define SMC_FNID_CRYPTO_SERV    0xC2000200

/* 运行时服务描述符 */
typedef struct {
    uint32_t start_fnid;
    uint32_t end_fnid;
    const char *name;
    uintptr_t (*setup)(void);
    uintptr_t (*invoke)(uint32_t, uintptr_t, uintptr_t,
                        uintptr_t, uintptr_t, void *);
} runtime_service_t;

/* 注册的运行时服务 */
static runtime_service_t services[] = {
    {
        .start_fnid = SMC_FNID_POWER_STATE,
        .end_fnid = SMC_FNID_SYSTEM_RESET,
        .name = "PSCI",
        .setup = psci_setup,
        .invoke = psci_smc_handler,
    },
    {
        .start_fnid = SMC_FNID_TPM_OP,
        .end_fnid = SMC_FNID_TPM_OP + 0xFF,
        .name = "TPM",
        .setup = tpm_service_setup,
        .invoke = tpm_smc_handler,
    },
    {
        .start_fnid = SMC_FNID_CRYPTO_SERV,
        .end_fnid = SMC_FNID_CRYPTO_SERV + 0xFF,
        .name = "Crypto",
        .setup = crypto_service_setup,
        .invoke = crypto_smc_handler,
    },
};

void bl31_main(void) {
    /* 1. 架构初始化 */
    enable_mmu_el3();

    /* 2. 配置中断控制器 */
    gic_init();
    gic_setup();

    /* 3. 初始化运行时服务 */
    for (int i = 0; i < ARRAY_SIZE(services); i++) {
        if (services[i].setup) {
            services[i].setup();
        }
    }

    /* 4. 初始化BL32（如果存在） */
    if (bl32_init_info) {
        bl32_init(bl32_init_info);
    }

    /* 5. 准备进入BL33 */
    INFO("Preparing to enter BL33 (Normal World)\n");

    /* 设置非安全世界上下文 */
    cm_init_context_by_index(NON_SECURE, bl33_ep_info);

    /* 配置安全状态 */
    cm_set_next_eret_context(NON_SECURE);

    /* 6. 进入非安全世界 */
    bl31_plat_runtime_setup();

    /* 进入BL33（不返回） */
    cm_eret_to_kernel();
}

/* SMC异常处理 - 汇编入口调用 */
uintptr_t smc_handler(uint32_t smc_fid,
                      uintptr_t x1, uintptr_t x2,
                      uintptr_t x3, uintptr_t x4,
                      void *cookie, void *handle,
                      uintptr_t flags) {
    /* 查找并调用对应的服务 */
    for (int i = 0; i < ARRAY_SIZE(services); i++) {
        if (smc_fid >= services[i].start_fnid &&
            smc_fid <= services[i].end_fnid) {
            return services[i].invoke(smc_fid, x1, x2, x3, x4, handle);
        }
    }

    /* 未知SMC */
    WARN("Unknown SMC: 0x%x\n", smc_fid);
    return SMC_UNK;
}
