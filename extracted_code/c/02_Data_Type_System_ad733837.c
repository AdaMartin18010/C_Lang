/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\02_Data_Type_System.md
 * Line: 871
 * Language: c
 * Block ID: ad733837
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ UNSAFE: 无符号下溢
void process_data(const char *data, size_t len, size_t offset) {
    size_t remaining = len - offset;  // 如果 offset > len，回绕到极大值！
    // 后续使用 remaining 可能导致越界
}

// ✅ SAFE: 检查前置条件
void process_data_safe(const char *data, size_t len, size_t offset) {
    if (offset > len) {
        // 错误处理
        return;
    }
    size_t remaining = len - offset;  // 现在安全
}

// ✅ SAFE: 使用有符号类型表示差值
ptrdiff_t remaining = (ptrdiff_t)len - (ptrdiff_t)offset;  // 可能为负
if (remaining < 0) {
    // 错误处理
}
