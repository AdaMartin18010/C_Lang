/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\02_Secure_Boot_Chain.md
 * Line: 456
 * Language: c
 * Block ID: 0acd9422
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 必须防止加载旧版本（有已知漏洞）的固件
// 解决方案：安全版本号计数器

int check_anti_rollback(uint32_t new_version) {
    uint32_t current_version = read_security_counter();

    if (new_version < current_version) {
        printf("Rollback detected! Current: %u, New: %u\n",
               current_version, new_version);
        return -1;
    }

    // 更新安全计数器（单调递增）
    if (new_version > current_version) {
        increment_security_counter();
    }

    return 0;
}
