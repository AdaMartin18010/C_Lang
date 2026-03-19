/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\10_MISRA_C_2023_Rules_10.md
 * Line: 116
 * Language: c
 * Block ID: e6a845ea
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 声明时初始化 */
int sum = 0;
int x = 5;
sum = sum + x;

/* ✅ 合规 - 所有分支初始化 */
int value;
if (condition) {
    value = 10;
} else {
    value = 0;  /* 确保初始化 */
}
use(value);

/* ✅ 合规 - C99在for中声明 */
for (int i = 0; i < 10; i++) { }  /* i作用域最小化 */

/* ✅ 合规 - 数组初始化 */
int arr[10] = {0};  /* 全部初始化为0 */
int arr2[10] = {1, 2, 3};  /* 剩余为0 */

/* ✅ 合规 - 结构体初始化 */
struct Point p = {0};  /* 全部成员初始化为0 */
struct Point p2 = {.x = 10, .y = 20};  /* 指定初始化 */

/* ✅ 合规 - 指针初始化为NULL */
int *p = NULL;
/* 稍后分配 */
p = malloc(sizeof(int));
if (p != NULL) {
    *p = 42;
}
