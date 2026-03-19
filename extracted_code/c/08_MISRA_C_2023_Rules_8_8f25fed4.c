/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 237
 * Language: c
 * Block ID: 8f25fed4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 未定义未定义的宏 */
#undef UNDEFINED_MACRO  /* 取消定义未定义的宏 */

/* ❌ 违反 - 条件依赖于未定义宏 */
#if UNDEFINED_MACRO > 0  /* 未定义宏在条件中使用 */
/* ... */
#endif
