/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\01_Standard_IO\README.md
 * Line: 565
 * Language: c
 * Block ID: 15482ec9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C11 线程安全函数 */
int fputs(const char *s, FILE *stream);  /* 线程安全 */
int fputc(int c, FILE *stream);          /* 线程安全 */

/* 但多字符操作的原子性需要额外考虑 */
