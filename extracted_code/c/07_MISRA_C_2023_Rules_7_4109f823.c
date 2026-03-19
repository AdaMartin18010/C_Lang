/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\07_MISRA_C_2023_Rules_7.md
 * Line: 268
 * Language: c
 * Block ID: 4109f823
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 对单个对象使用指针算术 */
int value = 42;
int *p = &value;
p++;  /* 未定义行为！ */

/* ❌ 违反 - 越界指针算术 */
int arr[10];
int *p = &arr[0];
p += 20;  /* 越界 */
*p = 0;   /* 写入未知内存 */

/* ❌ 违反 - 不同类型的指针运算 */
void *vp = arr;
vp += sizeof(int);  /* void*上的算术是GCC扩展，非标准C */

/* ❌ 违反 - 未初始化的指针算术 */
int *p;
p += 5;  /* 未定义行为 */
