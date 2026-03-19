/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\11_MISRA_C_2023_Rules_11.md
 * Line: 262
 * Language: c
 * Block ID: 65a5a103
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 严重违反 - 未定义行为 */
int x = i++ + i++;  /* 未定义！ */

/* ❌ 违反 - 函数调用顺序不确定 */
int r = func1() + func2();  /* 哪个先调用不确定 */

/* ❌ 违反 - 参数求值顺序不确定 */
result = calculate(a++, b++, a + b);  /* 不确定！ */

/* ❌ 违反 - 自修改 */
arr[i] = i++;  /* 未定义！ */
