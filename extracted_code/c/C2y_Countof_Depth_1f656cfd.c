/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 260
 * Language: c
 * Block ID: 1f656cfd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int arr[10];
int *p = arr;

_Countof(arr);   // 正确：返回 10
// _Countof(p);  // 编译错误：p是指针，不是数组

// 动态分配的"数组"
int *dyn_arr = malloc(10 * sizeof(int));
// _Countof(dyn_arr);  // 编译错误：无法确定元素数
