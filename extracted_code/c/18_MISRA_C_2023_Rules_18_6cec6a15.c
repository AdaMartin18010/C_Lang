/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\18_MISRA_C_2023_Rules_18.md
 * Line: 195
 * Language: c
 * Block ID: 6cec6a15
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 安全字符串复制 */
#include <string.h>

void safe_strcpy(char *dest, size_t dest_size, const char *src)
{
    if (dest == NULL || src == NULL || dest_size == 0) {
        return;
    }

    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';  /* 确保终止 */
}

/* ✅ 安全字符串连接 */
void safe_strcat(char *dest, size_t dest_size, const char *src)
{
    if (dest == NULL || src == NULL || dest_size == 0) {
        return;
    }

    size_t dest_len = strlen(dest);
    if (dest_len >= dest_size - 1) {
        return;  /* 已满 */
    }

    strncat(dest, src, dest_size - dest_len - 1);
}

/* ✅ 格式化字符串 */
void safe_format(char *buf, size_t buf_size, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, buf_size, fmt, args);
    va_end(args);

    buf[buf_size - 1] = '\0';  /* 确保终止 */
}

/* ✅ 使用安全函数 */
#include <stdio.h>

void read_line_safe(char *buffer, size_t size)
{
    if (fgets(buffer, (int)size, stdin) != NULL) {
        /* 移除可能的换行符 */
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}
