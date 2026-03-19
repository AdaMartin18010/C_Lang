/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 56
 * Language: c
 * Block ID: d8f927bf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(arr[0])  // 错误！缺少括号

int arr[10];
size_t n = ARRAY_SIZE(arr) * 2;  // 展开为: sizeof(arr) / sizeof(arr[0]) * 2
// 实际计算: (sizeof(arr) / sizeof(arr[0])) * 2，但由于优先级问题，结果可能错误
