/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\04_C17_C23_Library.md
 * Line: 413
 * Language: c
 * Block ID: da687cf8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifdef __STDC_LIB_EXT1__

#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void bounds_checked_functions(void) {
    char dest[100];
    errno_t err;

    // 安全字符串复制
    err = strcpy_s(dest, sizeof(dest), "safe copy");
    if (err != 0) {
        // 处理错误
    }

    // 安全字符串连接
    err = strcat_s(dest, sizeof(dest), " - appended");

    // 安全格式化
    err = sprintf_s(dest, sizeof(dest), "Value: %d", 42);

    // 安全gets替代
    err = gets_s(dest, sizeof(dest));  // 最多读99字符+null
}

#endif  // __STDC_LIB_EXT1__
