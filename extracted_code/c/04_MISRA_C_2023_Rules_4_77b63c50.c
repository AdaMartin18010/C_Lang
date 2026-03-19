/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\04_MISRA_C_2023_Rules_4.md
 * Line: 328
 * Language: c
 * Block ID: 77b63c50
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 相同typedef名称不同定义 */
/* file1.h */
typedef int status_t;

/* file2.h */
typedef enum { OK, ERROR } status_t;  /* 冲突！ */

/* 同时包含时会导致未定义行为 */
