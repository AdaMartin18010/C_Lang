/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\10_MISRA_C_2023_Rules_10.md
 * Line: 503
 * Language: c
 * Block ID: 082d1ec3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 类型不兼容的声明 */
/* file1.c */
int count = 0;

/* file2.c */
extern short count;  /* 类型不兼容！ */

/* ❌ 违反 - 符号性不匹配 */
/* header.h */
extern int value;

/* file.c */
unsigned int value = 0;  /* 符号性不匹配 */

/* ❌ 违反 - 指针类型不匹配 */
extern char *buffer;
char buffer[100];  /* 数组vs指针，在某些上下文有问题 */
