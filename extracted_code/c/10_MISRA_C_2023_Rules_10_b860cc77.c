/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\10_MISRA_C_2023_Rules_10.md
 * Line: 83
 * Language: c
 * Block ID: b860cc77
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 未初始化读取 */
int sum;
int x = 5;
sum = sum + x;  /* sum未初始化！ */

/* ❌ 违反 - 条件初始化 */
int value;
if (condition) {
    value = 10;
}
use(value);  /* 可能未初始化！ */

/* ❌ 违反 - 循环中的变量 */
int i;
for (i = 0; i < 10; i++) { }  /* 虽然安全，但最好在for中声明 */

/* ❌ 违反 - 数组未初始化 */
int arr[10];
int x = arr[0];  /* 未初始化读取 */

/* ❌ 违反 - 结构体部分初始化 */
struct Point p;
p.x = 10;
use(p.y);  /* y未初始化！ */

/* ❌ 违反 - 指针未初始化 */
int *p;
*p = 42;  /* 未定义行为！ */
