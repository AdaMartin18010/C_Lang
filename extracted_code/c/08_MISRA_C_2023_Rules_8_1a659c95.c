/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 325
 * Language: c
 * Block ID: 1a659c95
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 不完整的条件 */
#ifdef FEATURE
/* 某些代码 */
/* 缺少#endif */

/* ❌ 违反 - 条件嵌套错误 */
#ifdef A
#ifdef B
#endif  /* 关闭B */
/* A仍然打开，可能导致意外包含 */
