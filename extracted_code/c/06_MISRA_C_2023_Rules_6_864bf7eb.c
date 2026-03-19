/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 128
 * Language: c
 * Block ID: 864bf7eb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 分离表达式 */
int i = 0;
int temp1 = i++;
int temp2 = i++;
int x = temp1 + temp2;  /* x = 0 + 1 = 1 */

/* ✅ 合规 - 避免副作用 */
int a = 5;
a = 3;
int b = a + a;  /* b = 6，明确 */

/* ✅ 合规 - 函数调用顺序明确 */
int f1 = func();
int f2 = func();
int result = f1 + f2;  /* result = 1 + 2 = 3 */

/* ✅ 合规 - 数组访问安全 */
int arr[10] = {0};
int i = 0;
int idx1 = i++;
int idx2 = i++;
int val = arr[idx1] + arr[idx2];
