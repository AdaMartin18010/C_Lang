/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\CERT_C_2024\README.md
 * Line: 142
 * Language: c
 * Block ID: 2540d9d4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 不合规代码 - 内存泄漏
int* create_buffer(void) {
    int* buf = malloc(100 * sizeof(int));
    if (buf == NULL) return NULL;

    if (some_error_condition()) {
        return NULL;  // 泄漏!
    }
    return buf;
}

// 合规代码
int* create_buffer_safe(void) {
    int* buf = malloc(100 * sizeof(int));
    if (buf == NULL) return NULL;

    if (some_error_condition()) {
        free(buf);  // 正确释放
        return NULL;
    }
    return buf;
}
