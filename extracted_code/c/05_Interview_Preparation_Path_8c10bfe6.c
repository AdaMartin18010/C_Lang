/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\05_Interview_Preparation_Path.md
 * Line: 129
 * Language: c
 * Block ID: 8c10bfe6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 关键区别：
// 1. sizeof结果不同
int arr[10];
int *p = arr;
printf("%zu %zu\n", sizeof(arr), sizeof(p));  // 40 8 (64位)

// 2. 数组名是常量指针，不能赋值
arr = NULL;  // 错误！
p = NULL;    // 正确

// 3. 数组在栈/数据段分配连续空间
//    指针只是存储地址的变量

// 4. 函数参数中，数组退化为指针
void f(int arr[10]) { /* arr实际上是指针 */ }
