/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 1892
 * Language: c
 * Block ID: 4b6cf205
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 无边界检查的字符串拷贝
void stack_overflow_strcpy(const char *input) {
    char buffer[64];
    strcpy(buffer, input);  // ❌ 如果input >= 64字节，栈溢出！
}

// ❌ 错误: 循环越界
void stack_overflow_loop(void) {
    int arr[10];
    for (int i = 0; i <= 10; i++) {  // ❌ 条件应该是 i < 10
        arr[i] = i;  // arr[10] 越界！
    }
}

// ❌ 错误: sprintf无限制
void stack_overflow_sprintf(int value) {
    char buffer[20];
    sprintf(buffer, "Value is: %d", value);  // ❌ value很大时溢出
}

// ✅ 正确: 使用安全函数
void stack_safe(const char *input, int value) {
    char buffer[64];

    // 方式1: 使用strncpy
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    // 方式2: 使用snprintf
    char buf2[20];
    snprintf(buf2, sizeof(buf2), "Value: %d", value);
}
