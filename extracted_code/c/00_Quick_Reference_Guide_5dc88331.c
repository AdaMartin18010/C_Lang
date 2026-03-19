/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\00_Quick_Reference_Guide.md
 * Line: 308
 * Language: c
 * Block ID: 5dc88331
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 错误 */
char buf[10];
strcpy(buf, long_string);  /* 溢出 */

/* ✅ 修复 */
char buf[10];
strncpy(buf, long_string, sizeof(buf) - 1);
buf[sizeof(buf) - 1] = '\0';
