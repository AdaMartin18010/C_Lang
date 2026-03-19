/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\20_MISRA_C_2023_Rules_20.md
 * Line: 29
 * Language: c
 * Block ID: 2e4ae620
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 安全字符串函数 */
#include <string.h>

/* 使用带长度限制的函数 */
char dest[100];
strncpy(dest, src, sizeof(dest) - 1);
dest[sizeof(dest) - 1] = '\0';

/* 使用strnlen */
size_t len = strnlen(src, MAX_LEN);

/* ✅ 安全格式化 */
#include <stdio.h>

char buf[256];
snprintf(buf, sizeof(buf), "%s: %d", name, value);

/* ✅ 安全内存操作 */
#include <string.h>

/* 检查重叠（memmove可处理重叠）*/
if (dest < src || dest >= src + n) {
    memcpy(dest, src, n);
} else {
    memmove(dest, src, n);
}
