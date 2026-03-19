/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 4036
 * Language: c
 * Block ID: c3119bda
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * security_best_practices.c
 * ESP32 安全编码最佳实践
 */

/**
 * @brief 禁用 JTAG 调试 (生产环境)
 */
static void disable_jtag(void)
{
    /* 在 menuconfig 中启用安全启动后会自动禁用 JTAG */
    /* 或手动禁用: */
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_PAD_JTAG);
}

/**
 * @brief 安全字符串处理
 */
static void secure_string_handling(void)
{
    char buffer[32];
    const char *input = "user_input";

    /* 错误: 可能导致缓冲区溢出 */
    /* strcpy(buffer, input); */

    /* 正确: 使用 strncpy 并确保 null 终止 */
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    /* 或更安全的版本 (C11) */
    strncpy_s(buffer, sizeof(buffer), input, sizeof(buffer) - 1);
}

/**
 * @brief 启用 Flash 加密
 */
/* sdkconfig:
CONFIG_SECURE_FLASH_ENC_ENABLED=y
CONFIG_SECURE_FLASH_ENCRYPTION_MODE_RELEASE=y
*/

/**
 * @brief 启用安全启动
 */
/* sdkconfig:
CONFIG_SECURE_BOOT=y
CONFIG_SECURE_BOOT_V2_ENABLED=y
CONFIG_SECURE_BOOT_SIGNING_KEY="secure_boot_signing_key.pem"
*/
