/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 249
 * Language: c
 * Block ID: 9e5d9c47
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 检查后再#undef */
#ifdef TEMP_MACRO
#undef TEMP_MACRO
#endif

/* ✅ 合规 - 定义默认值 */
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

/* ✅ 合规 - 使用defined运算符 */
#if defined(DEBUG) && DEBUG > 0
/* 调试代码 */
#endif
