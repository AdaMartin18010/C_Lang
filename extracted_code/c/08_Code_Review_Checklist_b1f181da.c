/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 256
 * Language: c
 * Block ID: b1f181da
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例：栈缓冲区溢出
void parse_header(const char *input) {
    char buffer[64];
    strcpy(buffer, input);  // 危险！input可能超过64字节
    // 攻击者可以覆盖返回地址
}

// ❌ 错误示例：堆缓冲区溢出
void process_items(int count) {
    int *items = malloc(count * sizeof(int));
    for (int i = 0; i <= count; i++) {  // 越界！应为 i < count
        items[i] = i;  // 最后一次写入越界
    }
}

// ✅ 正确示例：安全字符串操作
void parse_header_safe(const char *input) {
    char buffer[64];
    // 方法1: 使用安全版本
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';  // 确保null终止

    // 方法2: 使用带长度检查的函数
    if (strlen(input) >= sizeof(buffer)) {
        log_error("Input too long");
        return;
    }
    memcpy(buffer, input, strlen(input) + 1);
}

// ✅ 正确示例：动态大小检查
void process_items_safe(int count) {
    if (count <= 0 || count > MAX_ITEMS) {
        return;  // 输入验证
    }
    int *items = malloc((size_t)count * sizeof(int));
    if (items == NULL) {
        return;  // 分配失败检查
    }
    for (int i = 0; i < count; i++) {  // 正确边界
        items[i] = i;
    }
    free(items);
}
