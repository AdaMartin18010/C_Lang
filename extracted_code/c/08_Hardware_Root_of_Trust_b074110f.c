/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 2342
 * Language: c
 * Block ID: b074110f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * Intel Trusted Execution Technology (TXT) 概述
 * 注：Intel TXT是x86平台的硬件信任根实现
 */

/*
 * Intel TXT组件：
 *
 * 1. SENTER/SEXIT指令
 *    - 安全启动测量环境
 *    - 建立动态信任根
 *
 * 2. TPM交互
 *    - 扩展PCR 17-22
 *    - 存储测量日志(SML)
 *
 * 3. 安全模式扩展(SMX)
 *    - GETSEC指令
 *    - 验证启动环境
 *
 * 4. 芯片组配置
 *    - DMA保护
 *    - 内存保护
 *
 * 由于Intel TXT是专有技术且主要在服务器/PC平台使用，
 * 嵌入式领域更常用ARM TrustZone或RISC-V PMP。
 *
 * 关键概念：
 * - 静态信任根：SRTM (Static Root of Trust for Measurement)
 * - 动态信任根：DRTM (Dynamic Root of Trust for Measurement)
 * - Measured Launch Environment (MLE)
 */

/* Intel TXT流程示意 */
void intel_txt_flow_diagram(void) {
    printf("Intel TXT Measured Launch Flow:\n\n");

    printf("1. 系统启动\n");
    printf("   ↓\n");
    printf("2. SINIT ACM (Authenticated Code Module)\n");
    printf("   - 验证芯片组配置\n");
    printf("   - 建立信任根\n");
    printf("   - 保护MLE内存\n");
    printf("   ↓\n");
    printf("3. 启动Measured Launch\n");
    printf("   GETSEC[SENTER]\n");
    printf("   ↓\n");
    printf("4. 验证MLE\n");
    printf("   - 测量MLE代码\n");
    printf("   - 扩展到TPM PCR 18\n");
    printf("   ↓\n");
    printf("5. 启动MLE\n");
    printf("   - 受保护的环境\n");
    printf("   - DMA保护启用\n");
}

/* 嵌入式平台的TXT替代方案 */
typedef enum {
    EMBEDDED_TXT_ARM_TRUSTZONE = 0,
    EMBEDDED_TXT_RISCV_PMP,
    EMBEDDED_TXT_TPM_DRTM,
    EMBEDDED_TXT_OPTEE,
} embedded_txt_equivalent_t;

/* 选择建议 */
const char* select_txt_equivalent(const char *platform) {
    if (strstr(platform, "ARM") || strstr(platform, "arm")) {
        return "ARM TrustZone + OP-TEE";
    }
    else if (strstr(platform, "RISC-V") || strstr(platform, "riscv")) {
        return "RISC-V PMP + MultiZone/Keystone";
    }
    else if (strstr(platform, "x86") || strstr(platform, "Intel")) {
        return "Intel TXT (if available) or TPM-based DRTM";
    }
    return "Unknown platform";
}
