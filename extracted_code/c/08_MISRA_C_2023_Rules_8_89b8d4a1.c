/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 392
 * Language: c
 * Block ID: 89b8d4a1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 使用gets */
char buf[100];
gets(buf);  /* 危险！ */

/* ❌ 违反 - 使用strcpy */
char dest[10];
strcpy(dest, "long string here");  /* 溢出！ */

/* ❌ 违反 - 使用sprintf */
char buf[10];
int x = 123456;
sprintf(buf, "%d", x);  /* 可能溢出 */
