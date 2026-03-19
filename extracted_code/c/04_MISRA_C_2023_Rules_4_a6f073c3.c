/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\04_MISRA_C_2023_Rules_4.md
 * Line: 249
 * Language: c
 * Block ID: a6f073c3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用前缀 */
#define MY_DEBUG_PRINT(x) printf("[DEBUG] %s\n", x)
#define MY_NULL_POINTER ((void*)0)

/* ✅ 合规 - 使用大写命名 */
#define BUFFER_SIZE 1024
#define MAX_RETRIES 3

/* ✅ 合规 - 条件编译保护 */
#ifndef MY_HEADER_H
#define MY_HEADER_H
/* ... */
#endif
