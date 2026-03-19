/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1419
 * Language: c
 * Block ID: aa2db34f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int arr[10];

// 表达式上下文（退化）
int *p = arr;           // arr 退化为 &arr[0]
func(arr);              // arr 退化为指针
arr + 1;                // 指针算术

// 例外情况（不退化）
sizeof(arr);            // 数组大小
_Countof(arr);          // 数组元素数
&arr;                   // 整个数组的地址
char copy[10] = arr;    // 初始化时（实际上是复制）
