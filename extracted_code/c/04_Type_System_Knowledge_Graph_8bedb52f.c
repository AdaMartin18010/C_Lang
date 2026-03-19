/*
 * Auto-generated from: 06_Thinking_Representation\07_Knowledge_Graph\04_Type_System_Knowledge_Graph.md
 * Line: 236
 * Language: c
 * Block ID: 8bedb52f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int *p;                    // p 是指向 int 的指针
int arr[10];               // arr 是包含 10 个 int 的数组
int *arr[10];              // arr 是包含 10 个 int* 的数组
int (*p)[10];              // p 是指向包含 10 个 int 的数组的指针
int *func();               // func 是返回 int* 的函数
int (*func)();             // func 是指向返回 int 的函数的指针
int (*arr[5])();           // arr 是包含 5 个函数指针的数组
int *(*(*func)[10])();     // 更复杂的声明...
