/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 243
 * Language: c
 * Block ID: 6a116666
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 情况一：数组作为函数参数
void func(int arr[]) {
    // arr 在这里是指针，不是数组
    // _Countof(arr);  // 编译错误
}

// 情况二：数组在表达式中（大多数情况）
int arr[10];
int *p = arr;  // arr退化为指向首元素的指针

// 情况三：取地址
int (*pa)[10] = &arr;  // &arr 是整个数组的地址，类型不同
