/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\03_String_Processing.md
 * Line: 134
 * Language: c
 * Block ID: 6d078657
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ strcat不安全
define BUF_SIZE 100
char buffer[BUF_SIZE] = "Hello";
strcat(buffer, " ");
strcat(buffer, "World");
// 如果总长超过BUF_SIZE，溢出！

// ✅ 安全连接
define BUF_SIZE 100
char buffer[BUF_SIZE] = "Hello";
strncat(buffer, " ", BUF_SIZE - strlen(buffer) - 1);
strncat(buffer, "World", BUF_SIZE - strlen(buffer) - 1);

// ✅ 更安全的实现
bool safe_concat(char *dest, const char *src, size_t size) {
    if (!dest || !src || size == 0) return false;

    size_t dest_len = strnlen(dest, size);
    if (dest_len >= size - 1) return false;  // 已满

    size_t remaining = size - dest_len - 1;
    strncpy(dest + dest_len, src, remaining);
    dest[size - 1] = '\0';
    return true;
}
