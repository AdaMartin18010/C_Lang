/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\04_C23_Memory_Security.md
 * Line: 269
 * Language: c
 * Block ID: cca6d2b8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 安全字符串复制
errno_t safe_strcpy(char *dst, rsize_t dstsz, const char *src) {
    if (dst == NULL || src == NULL) return EINVAL;
    if (dstsz == 0) return ERANGE;

    rsize_t i;
    for (i = 0; i < dstsz - 1 && src[i] != '\0'; i++) {
        dst[i] = src[i];
    }
    dst[i] = '\0';

    return (src[i] == '\0') ? 0 : ERANGE;  // 截断检查
}

// 更安全的替代: 使用已知长度
void process_string(const char *input, size_t len) {
    char buffer[256];
    if (len >= sizeof(buffer)) {
        // 输入过长，拒绝或截断
        return;
    }
    memcpy(buffer, input, len);
    buffer[len] = '\0';
    // 处理buffer...
}
