/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\README.md
 * Line: 52
 * Language: c
 * Block ID: 3a39042a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 避免未定义行为 */
/* 不明确定表达式求值顺序 */
int a = func1();
int b = func2();
int result = a + b;  /* 明确 */

/* ✅ 避免整数溢出 */
if (a > INT_MAX - b) {
    /* 处理溢出 */
} else {
    int sum = a + b;  /* 安全 */
}
