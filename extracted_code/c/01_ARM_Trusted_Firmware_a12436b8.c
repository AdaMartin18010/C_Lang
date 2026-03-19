/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\01_ARM_Trusted_Firmware.md
 * Line: 404
 * Language: c
 * Block ID: a12436b8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 不检查版本
if (verify_signature(image)) {
    boot_image(image);  // 可能回滚到旧版本漏洞
}

// ✅ 安全版本号检查
typedef struct {
    uint32_t version;
    uint32_t security_patch;
    // ...
} AntiRollback;

bool check_rollback(uint32_t new_version) {
    uint32_t current = read_persistent_version();
    if (new_version < current) {
        // 回滚尝试！
        log_security_event(ROLLBACK_ATTEMPT);
        return false;
    }
    update_persistent_version(new_version);
    return true;
}
