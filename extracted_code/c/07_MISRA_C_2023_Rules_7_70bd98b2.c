/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\07_MISRA_C_2023_Rules_7.md
 * Line: 189
 * Language: c
 * Block ID: 70bd98b2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 使用已释放内存 */
int *p = malloc(sizeof(int));
free(p);
*p = 42;  /* 使用已释放内存！ */

/* ❌ 违反 - 未初始化指针 */
int *p;
*p = 42;  /* 未定义行为！ */

/* ❌ 违反 - 返回局部变量地址 */
int *get_local(void)
{
    int local = 42;
    return &local;  /* 返回局部变量地址！ */
}

/* ❌ 违反 - 越界指针解引用 */
int arr[10];
int *end = &arr[10];  /* 允许：指向末尾后一个位置 */
int *beyond = &arr[11];  /* 未定义行为 */
*beyond = 0;  /* 危险！ */
