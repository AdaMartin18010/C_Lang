/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\CERT_C_2024\README.md
 * Line: 171
 * Language: c
 * Block ID: b22aad32
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 不合规代码 - 未定义行为
void modify_string(void) {
    char* str = "hello";  // 字符串字面量
    str[0] = 'H';         // 崩溃或不可预测行为
}

// 合规代码
void safe_string_modify(void) {
    char str[] = "hello";  // 字符数组，可修改
    str[0] = 'H';          // 安全

    // 或使用动态分配
    char* str2 = strdup("hello");
    if (str2 != NULL) {
        str2[0] = 'H';
        free(str2);
    }
}
