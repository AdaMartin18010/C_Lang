/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\02_Undefined_Behavior.md
 * Line: 216
 * Language: c
 * Block ID: f1d979be
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 依赖有符号溢出回绕
int hash(int x) {
    return x * 31 + 1;  // 如果溢出，依赖特定行为
}

// ✅ 使用无符号类型
unsigned int hash_safe(unsigned int x) {
    return x * 31 + 1;  // 明确定义的回绕
}
