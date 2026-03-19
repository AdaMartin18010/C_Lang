/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\CERT_C_2024\README.md
 * Line: 194
 * Language: c
 * Block ID: 92d448e4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 不合规代码 - 非空终止字符串
void copy_string(char* src, char* dest, size_t n) {
    strncpy(dest, src, n);  // 如果 src >= n，dest 可能不终止
    // 未显式设置终止符
}

// 合规代码
void copy_string_safe(const char* src, char* dest, size_t n) {
    if (src == NULL || dest == NULL || n == 0) return;

    strncpy(dest, src, n - 1);  // 留一个位置给 '\0'
    dest[n - 1] = '\0';         // 确保终止
}
