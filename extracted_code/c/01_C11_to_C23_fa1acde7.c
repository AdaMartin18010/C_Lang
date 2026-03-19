/*
 * Auto-generated from: 07_Modern_Toolchain\12_Modern_C_Standards\01_C11_to_C23.md
 * Line: 837
 * Language: c
 * Block ID: fa1acde7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C11边界检查接口 */
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>

/* 安全字符串操作 */
void safe_string_ops(void) {
    char dest[100];
    const char* src = "Hello, World!";

    /* 安全拷贝 */
    errno_t err = strncpy_s(dest, sizeof(dest), src, sizeof(dest) - 1);
    if (err != 0) {
        /* 处理错误 */
    }

    /* 安全拼接 */
    strcat_s(dest, sizeof(dest), "!!!");

    /* 安全格式化 */
    sprintf_s(dest, sizeof(dest), "Value: %d", 42);
}

/* C23静态分析标注 */
[[gsl::suppress(bounds.1)]]
void legacy_buffer_access(void) {
    /* 抑制GSL边界检查警告 */
    char buf[10];
    /* 可能有问题的访问 */
}

/* 内存安全 */
[[clang::ownership_returns(malloc)]]
void* safe_malloc(size_t size);

[[clang::ownership_takes(malloc, 1)]]
void safe_free(void* ptr);
