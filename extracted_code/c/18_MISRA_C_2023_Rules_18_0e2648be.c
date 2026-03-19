/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\18_MISRA_C_2023_Rules_18.md
 * Line: 151
 * Language: c
 * Block ID: 0e2648be
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 不同数组比较 */
int arr1[10], arr2[10];
if (&arr1[0] < &arr2[0]) {  /* 未定义行为 */ }

/* ❌ 违反 - 对象地址比较 */
int x, y;
if (&x < &y) {  /* 未定义行为 */ }
