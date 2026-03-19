/*
 * Auto-generated from: 03_System_Technology_Domains\03_JIT_Regex.md
 * Line: 387
 * Language: c
 * Block ID: 9df14f60
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 自适应选择策略
bool should_use_jit(const char *pattern, size_t input_len) {
    // 短输入使用解释执行
    if (input_len < 100) return false;

    // 复杂模式值得JIT
    if (strchr(pattern, '|') || strchr(pattern, '*')) {
        return true;
    }

    return false;
}
