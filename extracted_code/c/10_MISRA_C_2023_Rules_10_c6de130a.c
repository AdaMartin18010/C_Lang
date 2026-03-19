/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\10_MISRA_C_2023_Rules_10.md
 * Line: 562
 * Language: c
 * Block ID: c6de130a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 严重违反 - 链接冲突 */
/* file1.c */
static int internal_var = 0;  /* 内部链接 */

/* file2.c */
extern int internal_var;  /* 试图外部链接！链接错误或不一致 */
