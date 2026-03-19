/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\10_MISRA_C_2023_Rules_10.md
 * Line: 612
 * Language: c
 * Block ID: 4a5e3f90
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 重复定义 */
/* file1.c */
int counter = 0;

/* file2.c */
int counter = 0;  /* 重复定义！链接错误 */

/* ❌ 违反 - 头文件中的定义 */
/* header.h */
int shared_value = 0;  /* 头文件中定义！ */

/* file1.c */
#include "header.h"  /* 定义1 */

/* file2.c */
#include "header.h"  /* 定义2 - 冲突！ */
