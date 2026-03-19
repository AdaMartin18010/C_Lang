/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\12_MISRA_C_2023_Rules_12.md
 * Line: 30
 * Language: c
 * Block ID: 0698d37f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 严重违反 - 赋值误用为相等 */
if (x = 0) {  /* 赋值，不是比较！ */
    /* 永远不会执行（x=0为假） */
}

/* ❌ 违反 - 复杂赋值 */
if (x = get_value() + offset) {
    /* 意图不明确 */
}

/* ❌ 违反 - 函数内赋值 */
if (result = process_data()) {
    /* 副作用隐藏在条件中 */
}

/* ❌ 违反 - switch中的赋值 */
switch (status = get_status()) {  /* 不清晰 */
    /* ... */
}
