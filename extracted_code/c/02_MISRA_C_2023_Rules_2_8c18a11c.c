/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 497
 * Language: c
 * Block ID: 8c18a11c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 条件编译中的未使用是允许的 */
#ifdef DEBUG
    #define DEBUG_PRINT(x) printf(x)
#else
    #define DEBUG_PRINT(x)  /* 在release中未使用，但允许 */
#endif

/* ✅ 合规 - 头文件保护 */
#ifndef HEADER_H
#define HEADER_H
/* ... */
#endif  /* HEADER_H 只在#ifndef中引用，但这是标准做法 */
