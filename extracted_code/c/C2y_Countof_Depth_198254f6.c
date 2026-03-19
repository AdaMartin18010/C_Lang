/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1482
 * Language: c
 * Block ID: 198254f6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 指针类型
int *p;
_Countof(p);  // 错误：p不是数组

// 动态分配
int *d = malloc(10 * sizeof(int));
_Countof(d);  // 错误：d是指针

// 函数参数（已退化）
void f(int arr[10]) {
    _Countof(arr);  // 错误：arr实际上是指针
}

// 非数组类型
_Countof(int);       // 错误
_Countof(struct S);  // 错误
