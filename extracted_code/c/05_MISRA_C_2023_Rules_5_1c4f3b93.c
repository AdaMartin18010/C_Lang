/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\05_MISRA_C_2023_Rules_5.md
 * Line: 525
 * Language: c
 * Block ID: 1c4f3b93
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 位域布局未文档化 */
struct flags {
    int a : 5;
    int b : 3;
    int c : 8;
};  /* 布局依赖于实现 */

/* ❌ 违反 - 有符号位域 */
struct bad {
    int sign : 1;  /* 有符号1位只能表示0/-1 */
};
