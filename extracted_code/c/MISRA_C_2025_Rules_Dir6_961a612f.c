/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 251
 * Language: c
 * Block ID: 961a612f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 声明的可见性控制 */

/* ❌ 违反 [待官方确认] */
/* 在头文件中定义变量 */
/* header.h */
int global_var = 0;  /* 违反：每个包含都定义 */

/* ✅ 合规 [待官方确认] */
/* header.h */
extern int global_var;  /* 声明 */

/* source.c */
int global_var = 0;  /* 定义 */
