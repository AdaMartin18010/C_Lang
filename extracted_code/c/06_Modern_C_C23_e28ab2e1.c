/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\06_Modern_C_C23.md
 * Line: 375
 * Language: c
 * Block ID: e28ab2e1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// VLA在C23中是可选特性
// 更推荐使用动态分配或固定大小

// 数组大小推导
int arr[] = {1, 2, 3, 4, 5};  // 自动推导为5

// 静态断言数组大小
static_assert(sizeof(arr) / sizeof(arr[0]) == 5);

// 多维数组增强
void func(int arr[static 10]);  // 承诺至少10个元素
