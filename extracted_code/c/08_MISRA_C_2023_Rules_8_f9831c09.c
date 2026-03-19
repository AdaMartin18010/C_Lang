/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 167
 * Language: c
 * Block ID: f9831c09
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 无保护的头文件 */
/* myheader.h */
void func(void);  /* 如果包含两次，会导致重复声明 */

/* ❌ 违反 - 不唯一的保护宏 */
/* file1.h */
#ifndef HEADER_H
#define HEADER_H
/* ... */
#endif

/* file2.h */
#ifndef HEADER_H  /* 与file1.h冲突！ */
#define HEADER_H
/* ... */
#endif
