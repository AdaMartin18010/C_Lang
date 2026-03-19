/*
 * Auto-generated from: 07_Modern_Toolchain\12_Modern_C_Standards\01_C11_to_C23.md
 * Line: 145
 * Language: c
 * Block ID: 2771bea0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 边界检查接口 (Annex K) */
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>

errno_t strncpy_s(char* dest, rsize_t destsz, const char* src, rsize_t count);
errno_t memcpy_s(void* dest, rsize_t destsz, const void* src, rsize_t count);

/* 使用 */
char buf[100];
strncpy_s(buf, sizeof(buf), source, sizeof(buf) - 1);

/* 静态分析标注 */
[[gsl::suppress(bounds.1)]]  /* 抑制特定警告 */
void legacy_code(void);
