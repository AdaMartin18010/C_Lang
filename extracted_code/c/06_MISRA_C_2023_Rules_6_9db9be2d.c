/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 99
 * Language: c
 * Block ID: 9db9be2d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 严重违反 - 未定义行为 */
int i = 0;
int x = i++ + i++;  /* 未定义行为！ */

/* ❌ 违反 - 求值顺序不确定 */
int a = 5;
int b = (a = 3) + a;  /* a的值不确定是3还是5 */

/* ❌ 违反 - 函数参数顺序不确定 */
int func(void) {
    static int count = 0;
    return ++count;
}
int result = func() + func();  /* 结果不确定是3还是其他值 */

/* ❌ 违反 - 数组下标副作用 */
int arr[10] = {0};
int i = 0;
int val = arr[i++] + arr[i++];  /* 未定义！ */

/* ❌ 违反 - 函数调用顺序不确定 */
int f(int x) { return x; }
int g(int x) { return x * 2; }
int result = f(i++) + g(i++);  /* 顺序不确定，结果不确定 */
