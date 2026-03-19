/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\18_MISRA_C_2023_Rules_18.md
 * Line: 103
 * Language: c
 * Block ID: 3a950daf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 不同数组 */
int arr1[10], arr2[10];
int diff = &arr1[5] - &arr2[5];  /* 未定义行为 */

/* ❌ 违反 - 对象和其后位置 */
int x, y;
size_t diff = &y - &x;  /* 未定义行为 */
