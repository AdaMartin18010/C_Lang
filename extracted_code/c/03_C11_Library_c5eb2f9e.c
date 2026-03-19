/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\03_C11_Library.md
 * Line: 204
 * Language: c
 * Block ID: c5eb2f9e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 可选支持，定义 __STDC_LIB_EXT1__
#ifdef __STDC_LIB_EXT1__

#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
#include <stdio.h>

// 安全字符串操作
errno_t strcpy_s(char *dest, rsize_t destsz, const char *src);
errno_t strncpy_s(char *dest, rsize_t destsz, const char *src, rsize_t count);

// 使用
char buffer[100];
strcpy_s(buffer, sizeof(buffer), "safe string copy");

// 安全格式化
sprintf_s(buffer, sizeof(buffer), "Value: %d", 42);

// 安全gets替代（有长度限制）
gets_s(buffer, sizeof(buffer));

#endif  // __STDC_LIB_EXT1__
