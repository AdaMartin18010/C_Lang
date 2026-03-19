/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\11_MISRA_C_2023_Rules_11.md
 * Line: 373
 * Language: c
 * Block ID: 761dfc99
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 整数作为布尔 */
if (count) {  /* 不明确：是非零检查？ */
    /* ... */
}

/* ❌ 违反 - 位运算误用为逻辑运算 */
if (flags & MASK) {  /* 位运算结果，不清晰 */
    /* ... */
}

/* ❌ 违反 - 指针隐式转布尔（某些风格不允许）*/
if (ptr) {  /* 虽然有效，但不明确 */
    /* ... */
}

/* ❌ 违反 - 比较结果作为整数 */
int result = (a > b) + (c > d);  /* 比较结果是0或1 */
