/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 278
 * Language: c
 * Block ID: d30d171a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 始终为真 */
if (1) {  /* 始终执行 */
    /* ... */
}

/* ❌ 违反 - 始终为假 */
if (0) {  /* 永远不会执行 */
    /* ... */
}

/* ❌ 违反 - 预处理导致的常量 */
#define DEBUG 0
if (DEBUG) {  /* 常量 */
    printf("Debug\n");
}

/* ❌ 违反 - switch常量 */
switch (1) {  /* 常量 */
    case 1: break;
}
