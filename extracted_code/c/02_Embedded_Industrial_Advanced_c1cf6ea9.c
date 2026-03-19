/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Industrial_Advanced.md
 * Line: 1080
 * Language: c
 * Block ID: c1cf6ea9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// ESP32安全启动流程
// 1. ROM验证Bootloader签名
// 2. Bootloader验证应用程序签名
// 3. 防回滚保护
// ============================================================

// 签名验证流程:
// 签名 = RSA/ECDSA_SIGN(PrivateKey, SHA256(Firmware))
// 验证 = RSA/ECDSA_Verify(PublicKey, Signature, SHA256(Firmware))

// efuse中烧录公钥摘要 (不可更改)
// SECURE_BOOT_EN = 1 (使能安全启动)
// ABS_DONE_0 = 1 (表示已烧录签名密钥)

// 使用espsecure.py工具生成签名
// espsecure.py sign_data --keyfile my_key.pem --output firmware_signed.bin firmware.bin

// 应用程序中的版本检查
#include "esp_ota_ops.h"

bool verify_app_version(void) {
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t running_app_info;

    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK) {
        ESP_LOGI(TAG, "Running firmware version: %s", running_app_info.version);

        // 检查版本号格式 (如 1.2.3)
        int major, minor, patch;
        if (sscanf(running_app_info.version, "%d.%d.%d",
                   &major, &minor, &patch) == 3) {
            // 版本检查逻辑...
        }
    }

    return true;
}

// 安全启动日志分析
void check_secure_boot_status(void) {
    // 检查efuse中的安全启动状态
    uint32_t secure_boot_en = REG_READ(EFUSE_SECURE_BOOT_EN);

    if (secure_boot_en) {
        ESP_LOGI(TAG, "Secure Boot: ENABLED");

        // 检查是否使用了硬件安全启动 (V2)
        uint32_t secure_boot_aggressive = REG_READ(EFUSE_SECURE_BOOT_AGGRESSIVE);
        if (secure_boot_aggressive) {
            ESP_LOGI(TAG, "Secure Boot Mode: AGGRESSIVE (Revoke after failed verify)");
        }
    } else {
        ESP_LOGE(TAG, "Secure Boot: DISABLED - SECURITY RISK!");
    }
}
