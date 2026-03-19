/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\16_MISRA_C_2023_Rules_16.md
 * Line: 31
 * Language: c
 * Block ID: d46944e4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 参数不匹配 */
void func(int a, int b);
func(1);        /* 参数太少 */
func(1, 2, 3);  /* 参数太多 */

/* ❌ 违反 - 省略号函数滥用 */
void bad_func(...);  /* 无固定参数，危险 */
bad_func();  /* 无参数 */
bad_func(1, "test", 3.14);  /* 随意传递 */
