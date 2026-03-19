/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\05_MISRA_C_2023_Rules_5.md
 * Line: 141
 * Language: c
 * Block ID: 4ce883af
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 编译时检测char符号性 */
#include <limits.h>

#if CHAR_MIN == 0
    #define CHAR_IS_UNSIGNED 1
    #pragma message("char is unsigned")
#else
    #define CHAR_IS_UNSIGNED 0
    #pragma message("char is signed")
#endif
