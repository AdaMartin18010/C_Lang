/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\18_MISRA_C_2023_Rules_18.md
 * Line: 30
 * Language: c
 * Block ID: e4526fec
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 越界访问 */
int arr[10];
int x = arr[10];  /* 越界！有效索引0-9 */

/* ❌ 违反 - 负数索引 */
int arr[10];
int x = arr[-1];  /* 未定义行为 */

/* ❌ 违反 - 用户输入未验证 */
void process(int index)
{
    int arr[10];
    arr[index] = 42;  /* 可能越界 */
}

/* ❌ 违反 - 循环边界错误 */
for (int i = 0; i <= 10; i++) {  /* 应该是 < 10 */
    arr[i] = 0;  /* i=10时越界 */
}

/* ❌ 违反 - 字符串越界 */
char str[5] = "hello";  /* 需要6字节（含'\0'）*/
