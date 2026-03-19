/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 102
 * Language: c
 * Block ID: da52dee9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 符合规范 - 使用安全变体 */
#ifdef __STDC_LIB_EXT1__
    #define __STDC_WANT_LIB_EXT1__ 1
    #include <string.h>

    errno_t err = strcpy_s(dest, sizeof(dest), src);
    if (err != 0) {
        /* 处理错误 */
    }
#endif
