/*
 * Auto-generated from: 06_Thinking_Representation\04_Application_Scenario_Trees\01_Industry_Application_Scenario_Tree.md
 * Line: 234
 * Language: c
 * Block ID: 24c02bfe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 不安全：分支依赖密钥
int insecure_compare(const uint8_t *a, const uint8_t *b, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (a[i] != b[i]) return 0;  // 时序泄露！
    }
    return 1;
}

// ✅ 安全：常量时间
int constant_time_compare(const uint8_t *a, const uint8_t *b, size_t n) {
    uint8_t result = 0;
    for (size_t i = 0; i < n; i++) {
        result |= a[i] ^ b[i];  // 无分支
    }
    return result == 0;
}
