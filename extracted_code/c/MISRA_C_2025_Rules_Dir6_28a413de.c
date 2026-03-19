/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 93
 * Language: c
 * Block ID: 28a413de
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 函数声明和定义的类型必须一致 */

/* ❌ 违反 [待官方确认] */
/* 声明和定义类型不一致 */
extern int func(int);
int func(short x) { /* ... */ }  /* 参数类型不匹配 */

/* ✅ 合规 [待官方确认] */
/* 声明和定义完全一致 */
extern int func(int x);
int func(int x) { /* ... */ }  /* 一致 */
