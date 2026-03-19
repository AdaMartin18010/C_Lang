/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\03_Measured_Boot.md
 * Line: 555
 * Language: c
 * Block ID: 690d9cc4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// measured_boot_demo.c
#include "tpm_measure.h"
#include "boot_verify.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 模拟系统组件
static const char *BOOT_COMPONENTS[] = {
    "uefi_firmware",
    "bootloader",
    "kernel",
    "initramfs",
    "kernel_modules",
    "systemd",
    "security_policy"
};

static const uint32_t BOOT_PCR_MAP[] = {0, 4, 8, 9, 9, 10, 11};

// 模拟启动过程度量
void simulate_boot_measurement(TPMContext *tpm) {
    printf("\n=== Simulating Boot Measurement Process ===\n\n");

    for (size_t i = 0; i < sizeof(BOOT_COMPONENTS)/sizeof(char*); i++) {
        // 生成组件的"度量值"（实际应为代码哈希）
        char component_data[256];
        snprintf(component_data, sizeof(component_data),
                 "Component: %s v1.0.0", BOOT_COMPONENTS[i]);

        // 扩展到对应PCR
        tpm_measure_buffer(tpm, BOOT_PCR_MAP[i],
                           (uint8_t*)component_data, strlen(component_data),
                           BOOT_COMPONENTS[i]);

        // 模拟启动延迟
        usleep(100000);
    }
}

// 显示所有PCR值
void display_all_pcrs(TPMContext *tpm) {
    uint8_t pcr_values[TPM_PCR_COUNT][SHA256_DIGEST_SIZE];

    printf("\n=== Current PCR Values ===\n\n");

    if (tpm_pcr_read_all(tpm, pcr_values) != 0) {
        fprintf(stderr, "Failed to read PCR values\n");
        return;
    }

    for (int i = 0; i < TPM_PCR_COUNT; i++) {
        bool is_zero = true;
        for (int j = 0; j < SHA256_DIGEST_SIZE; j++) {
            if (pcr_values[i][j] != 0) {
                is_zero = false;
                break;
            }
        }

        if (!is_zero) {
            printf("PCR[%02d]: ", i);
            for (int j = 0; j < SHA256_DIGEST_SIZE; j++) {
                printf("%02x", pcr_values[i][j]);
            }
            printf("\n");
        }
    }
}

int main(int argc, char *argv[]) {
    TPMContext tpm;

    printf("=== Measured Boot Demo ===\n");
    printf("Using TPM2 TSS2-ESAPI\n\n");

    // 初始化TPM
    if (tpm_init(&tpm) != 0) {
        fprintf(stderr, "TPM initialization failed. Using simulation mode.\n");
        // 继续演示，但使用模拟数据
    }

    // 模拟启动度量
    simulate_boot_measurement(&tpm);

    // 显示PCR值
    display_all_pcrs(&tpm);

    // 读取事件日志
    uint8_t event_log[TPM_EVENT_LOG_MAX];
    size_t log_size;
    if (tpm_read_event_log(event_log, &log_size) == 0) {
        tpm_dump_event_log(event_log, log_size);
    }

    // 创建验证上下文并验证
    VerifyContext *verify = verify_context_new();

    // 添加一些模拟的黄金参考值
    uint8_t dummy_hash[SHA256_DIGEST_SIZE];
    memset(dummy_hash, 0xAB, SHA256_DIGEST_SIZE);
    verify_add_reference(verify, 0, dummy_hash, "UEFI Firmware");

    verify_boot_integrity(verify, &tpm);

    // 清理
    verify_context_free(verify);
    tpm_cleanup(&tpm);

    return 0;
}

// 编译说明:
// gcc -o measured_boot measured_boot_demo.c tpm_measure.c boot_verify.c \
//     -ltss2-esys -ltss2-tctildr -lcrypto -lssl
