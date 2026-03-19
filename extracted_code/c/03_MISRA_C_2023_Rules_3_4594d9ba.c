/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\03_MISRA_C_2023_Rules_3.md
 * Line: 76
 * Language: c
 * Block ID: 4594d9ba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 Rule 3.1 - 块注释中包含 /* */
/* This is a comment with /* nested */ marker */
/* 编译器会将第一个 */ 视为注释结束，留下语法错误 */

/* ❌ 违反 Rule 3.1 - 块注释中包含 // */
/* This comment has // inside it */
/* 虽然某些编译器允许，但不符合标准 */

/* ❌ 违反 Rule 3.1 - 行注释中包含代码 */
// Comment with code: x = 5; /* This will cause issues */
/* 行注释中的 /* 不会开始块注释，但代码质量差 */

/* ❌ 违反 - URL在注释中被误解 */
/* Visit http://example.com/path */
/* 这里的 // 可能被某些工具误处理 */
