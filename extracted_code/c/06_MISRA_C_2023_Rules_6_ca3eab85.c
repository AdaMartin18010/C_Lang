/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 715
 * Language: c
 * Block ID: ca3eab85
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 逗号运算符 */
int x = (a = 1, b = 2, a + b);  /* x = 3 */

/* ❌ 违反 - for中的逗号 */
for (int i = 0, j = 0; i < 10; i++, j++) {
    /* 逗号运算符 */
}

/* ❌ 违反 - 函数参数中的逗号混淆 */
func((a, b), c);  /* 是传两个参数还是逗号运算符？ */
