/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\04_C23_Memory_Security.md
 * Line: 299
 * Language: c
 * Block ID: 186287b4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 指针验证模式
void process_buffer(uint8_t *data, size_t len) {
    // 1. NULL检查
    if (data == NULL) {
        // C23: 使用nullptr
        return;
    }

    // 2. 空长度检查
    if (len == 0) return;

    // 3. 溢出检查
    if (len > SIZE_MAX / 2) return;  // 防止后续计算溢出

    // 4. 边界内访问
    for (size_t i = 0; i < len; i++) {
        data[i] = process_byte(data[i]);
    }
}
