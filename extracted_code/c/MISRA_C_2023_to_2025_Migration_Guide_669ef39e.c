/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2023_to_2025_Migration_Guide.md
 * Line: 170
 * Language: c
 * Block ID: 669ef39e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C17 代码示例 */
#define MAX_SIZE 100
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void* allocate(size_t size);
int* p = NULL;

/* 预期的 C23 更新 [待官方文档确认] */
constexpr int MAX_SIZE = 100;           // 替代 #define
#define MIN(a, b) ((a) < (b) ? (a) : (b))  // 可能保持

void* allocate(size_t size);
int* p = nullptr;                       // 替代 NULL
