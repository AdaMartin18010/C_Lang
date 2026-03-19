/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\05_Interview_Preparation_Path.md
 * Line: 193
 * Language: c
 * Block ID: a141c627
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 安全版本，返回目标指针
char *my_strcpy(char *dest, const char *src) {
    assert(dest != NULL && src != NULL);
    char *d = dest;
    while ((*d++ = *src++) != '\0');
    return dest;
}

// 限制长度的安全版本
char *my_strncpy(char *dest, const char *src, size_t n) {
    assert(dest != NULL && src != NULL);
    char *d = dest;
    while (n-- > 0 && (*d++ = *src++) != '\0');
    // 如果src长度小于n，填充剩余空间
    while (n-- > 0) *d++ = '\0';
    return dest;
}

// 更好的实现（处理非终止情况）
size_t my_strlcpy(char *dest, const char *src, size_t size) {
    size_t src_len = strlen(src);
    if (size > 0) {
        size_t copy_len = (src_len >= size) ? size - 1 : src_len;
        memcpy(dest, src, copy_len);
        dest[copy_len] = '\0';
    }
    return src_len;
}
