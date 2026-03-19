/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\03_C23_Core_Features.md
 * Line: 326
 * Language: c
 * Block ID: ef348d56
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 允许空初始化器
int arr[10] = {};           // 所有元素初始化为 0
struct Point { int x, y; };
struct Point p = {};        // x 和 y 初始化为 0

// 与 memset(0) 等价，但更高效（编译时完成）
// 在 BSS 段的数据自动为零初始化
