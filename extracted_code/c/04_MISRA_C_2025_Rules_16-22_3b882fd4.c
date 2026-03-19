/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 14
 * Language: c
 * Block ID: 3b882fd4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 不符合规范 */
char buffer[10];
strcpy(buffer, "Hello World!");  /* 缓冲区溢出 - 未定义行为 */

/* 符合规范 */
char buffer[10];
strncpy(buffer, "Hello World!", sizeof(buffer) - 1);
buffer[sizeof(buffer) - 1] = '\0';
