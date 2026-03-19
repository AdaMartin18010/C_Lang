/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\02_Speculative_Execution.md
 * Line: 303
 * Language: c
 * Block ID: 001a64b4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 安全数组访问模式

// 方法1：强制边界检查
uint8_t safe_array_access(uint8_t *arr, size_t len, size_t index) {
    // 即使推测执行，也不能越界
    volatile size_t safe_index = index;
    if (safe_index >= len) {
        safe_index = 0;
    }
    return arr[safe_index];
}

// 方法2：使用掩码
uint8_t masked_array_access(uint8_t *arr, size_t len, size_t index) {
    // 计算掩码：如果index < len，mask = ~0；否则mask = 0
    size_t mask = -(index < len);  // 编译为条件移动或sbb
    size_t safe_index = index & mask;
    return arr[safe_index];
}

// 方法3：复制到固定大小缓冲区
uint8_t copy_then_access(uint8_t *arr, size_t len, size_t index) {
    uint8_t local[256] = {0};
    size_t copy_len = len < 256 ? len : 256;
    memcpy(local, arr, copy_len);

    if (index < 256) {
        return local[index];  // 总是安全的
    }
    return 0;
}
