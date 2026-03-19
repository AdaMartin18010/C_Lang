/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\03_MISRA_C_2023_Rules_3.md
 * Line: 161
 * Language: c
 * Block ID: 5f642283
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 Rule 3.2 - 行注释中的行拼接 */
// This is a comment \
   that continues on the next line

/* 实际效果： */
/* 第1行: // This is a comment that continues on the next line */
/* 第2行: 空的，但可能被视为代码！ */

/* 危险示例 */
int x;  // Variable x \
int y;  // This line is now commented out too!

/* 实际效果： */
/* int x;  // Variable x int y; */
/* 注意：int y; 也被注释掉了！ */
