/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\README.md
 * Line: 185
 * Language: c
 * Block ID: 96f5dbe0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// cdecl: 声明解析器
// 规则：从标识符开始，先右后左，遇到括号转向

int* arr[10];           // arr 是包含10个 int* 的数组
int (*arr)[10];         // arr 是指向包含10个 int 的数组的指针
int* (*arr)[10];        // arr 是指向包含10个 int* 的数组的指针

int (*func)(int);       // func 是指向函数的指针
int (*func_arr[5])(int); // func_arr 是包含5个函数指针的数组
int (*(*func_ptr_arr)[5])(int); // func_ptr_arr 是指向上述数组的指针

// 函数返回函数指针
void (*signal(int sig, void (*handler)(int)))(int);

// 使用 typedef 简化
typedef void (*sighandler_t)(int);
sighandler_t signal(int sig, sighandler_t handler);
