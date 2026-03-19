/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\15_MISRA_C_2023_Rules_15.md
 * Line: 109
 * Language: c
 * Block ID: a50e1fed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 有符号/无符号混合 */
int a = -1;
unsigned int b = 1;
if (a < b) {  /* false！a被转换为无符号大数 */
    /* ... */
}

/* ❌ 违反 - 混合运算 */
int x = 10;
unsigned int y = 20;
unsigned int z = x - y;  /* 无符号下溢 */

/* ❌ 违反 - 循环条件 */
for (int i = 0; i < count; i++) {  /* count如果是unsigned，混合 */
    /* ... */
}
