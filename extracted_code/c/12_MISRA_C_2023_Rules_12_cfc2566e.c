/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\12_MISRA_C_2023_Rules_12.md
 * Line: 265
 * Language: c
 * Block ID: cfc2566e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 多个出口 */
while (condition1) {
    if (condition2) {
        break;  /* 出口1 */
    }
    if (condition3) {
        return;  /* 出口2 */
    }
    /* ... */
    if (done) {
        break;  /* 出口3 */
    }
}

/* ❌ 违反 - 复杂退出条件 */
while (1) {
    /* ... */
    if (a && b || c && d) {
        break;
    }
}
