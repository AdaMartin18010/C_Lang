/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 24
 * Language: c
 * Block ID: ce8998db
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23之前的传统做法
int arr[10];
size_t count = sizeof(arr) / sizeof(arr[0]);  // 计算元素个数
