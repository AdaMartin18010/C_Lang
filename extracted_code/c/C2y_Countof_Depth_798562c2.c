/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 107
 * Language: c
 * Block ID: 798562c2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int arr[10];

sizeof(arr);       // 返回 40（假设int为4字节）
_Countof(arr);     // 返回 10（元素个数）

sizeof(int[10]);   // 返回 40
_Countof(int[10]); // 返回 10
