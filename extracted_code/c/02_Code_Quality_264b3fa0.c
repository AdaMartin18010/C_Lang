/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\02_Code_Quality.md
 * Line: 71
 * Language: c
 * Block ID: 264b3fa0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 未验证输入
void process_user_input(const char *input) {
    char buffer[100];
    strcpy(buffer, input);  // 可能溢出
}

// ✅ 长度验证
define MAX_INPUT 99

bool process_user_input_safe(const char *input) {
    if (input == NULL) return false;

    size_t len = strnlen(input, MAX_INPUT + 1);
    if (len > MAX_INPUT) {
        log_error("Input too long: %zu", len);
        return false;
    }

    char buffer[MAX_INPUT + 1];
    memcpy(buffer, input, len + 1);
    // 处理...
    return true;
}

// ✅ 严格解析（白名单）
bool parse_positive_int(const char *str, int *out) {
    if (str == NULL || out == NULL) return false;
    if (str[0] == '\0') return false;

    errno = 0;
    char *end;
    long val = strtol(str, &end, 10);

    // 检查完整消耗
    if (*end != '\0') return false;

    // 检查范围
    if (val <= 0 || val > INT_MAX) return false;

    // 检查溢出
    if (errno == ERANGE) return false;

    *out = (int)val;
    return true;
}
