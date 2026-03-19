/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\01_ARM_Trusted_Firmware.md
 * Line: 201
 * Language: c
 * Block ID: 28937567
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Secure Monitor Call (SMC) 处理

// SMC调用号定义
#define SMC_FID_MASK        0xFFFF
#define SMC_TA_MASK         0x3F000000
#define SMC_TA_ARM_STD      0x00000000
#define SMC_TA_ARM_FAST     0x01000000
#define SMC_TA_VENDOR_HYP   0x02000000
#define SMC_TA_VENDOR_SPR   0x03000000
#define SMC_TA_VENDOR_OS    0x04000000
#define SMC_TA_TRUSTED_APP  0x30000000
#define SMC_TA_TRUSTED_OS   0x32000000

// SMC调用参数
typedef struct {
    uint64_t x0;  // 功能ID
    uint64_t x1;
    uint64_t x2;
    uint64_t x3;
    uint64_t x4;
    uint64_t x5;
    uint64_t x6;
    uint64_t x7;
} SmcArgs;

typedef struct {
    uint64_t x0;
    uint64_t x1;
    uint64_t x2;
    uint64_t x3;
} SmcRet;

// SMC处理函数
SmcRet smc_handler(SmcArgs *args) {
    uint32_t fid = args->x0;
    SmcRet ret = {0};

    switch (fid) {
        // PSCI电源管理调用
        case PSCI_VERSION:
            ret.x0 = PSCI_MAJOR_VERSION << 16 | PSCI_MINOR_VERSION;
            break;

        case PSCI_CPU_ON:
            ret.x0 = psci_cpu_on(args->x1, args->x2, args->x3);
            break;

        case PSCI_CPU_OFF:
            ret.x0 = psci_cpu_off();
            break;

        case PSCI_SYSTEM_RESET:
            system_reset();
            break;

        // Trusted OS调用
        case OPTEE_SMC_CALLS_COUNT:
        case OPTEE_SMC_CALLS_UID:
        case OPTEE_SMC_CALLS_REVISION:
            // 转发给OP-TEE
            ret = forward_to_optee(args);
            break;

        default:
            ret.x0 = SMC_UNK;
            break;
    }

    return ret;
}

// 触发SMC调用（C接口）
static inline SmcRet smc_call(uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3) {
    SmcRet ret;

    __asm__ volatile (
        "mov x0, %[x0]\n\t"
        "mov x1, %[x1]\n\t"
        "mov x2, %[x2]\n\t"
        "mov x3, %[x3]\n\t"
        "smc #0\n\t"
        "mov %[r0], x0\n\t"
        "mov %[r1], x1\n\t"
        "mov %[r2], x2\n\t"
        "mov %[r3], x3\n\t"
        : [r0] "=r" (ret.x0),
          [r1] "=r" (ret.x1),
          [r2] "=r" (ret.x2),
          [r3] "=r" (ret.x3)
        : [x0] "r" (x0),
          [x1] "r" (x1),
          [x2] "r" (x2),
          [x3] "r" (x3)
        : "x0", "x1", "x2", "x3", "memory"
    );

    return ret;
}
