/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\01_ARM_Trusted_Firmware.md
 * Line: 389
 * Language: c
 * Block ID: 593d5737
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 硬编码密钥
const uint8_t private_key[] = {0x12, 0x34, ...};  // 可被提取！

// ✅ 使用硬件安全模块(HSM)或eFuse
// 密钥存储在一次性可编程eFuse中
#define EFUSE_KEY_ADDR 0x10A00

uint8_t *get_device_key(void) {
    return (uint8_t *)EFUSE_KEY_ADDR;  // 只读，无法导出
}
