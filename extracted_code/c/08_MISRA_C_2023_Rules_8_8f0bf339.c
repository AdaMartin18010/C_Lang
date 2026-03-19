/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 508
 * Language: c
 * Block ID: 8f0bf339
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 直接使用errno */
FILE *fp = fopen("file", "r");
if (errno == ENOENT) {  /* 错误！fopen可能未设置errno */
    /* ... */
}

/* ❌ 违反 - 不检查返回值 */
strtol(str, NULL, 10);
if (errno == ERANGE) {  /* 可能检查的是之前的错误 */
    /* ... */
}
