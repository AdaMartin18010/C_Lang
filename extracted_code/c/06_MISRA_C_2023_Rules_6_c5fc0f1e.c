/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 730
 * Language: c
 * Block ID: c5fc0f1e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 分离语句 */
a = 1;
b = 2;
int x = a + b;

/* ✅ 合规 - for中使用多个变量声明 */
int i, j;
for (i = 0, j = 0; i < 10; i++, j++) {  /* 允许在for中 */
    /* ... */
}
/* 注：MISRA允许for语句中的逗号，但不建议在其他地方使用 */

/* ✅ 合规 - 明确的函数调用 */
int temp = a;
func(temp, c);  /* 明确 */
