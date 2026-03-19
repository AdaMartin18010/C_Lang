/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\CISA_2026_Mandate_Depth.md
 * Line: 1027
 * Language: c
 * Block ID: e211819c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* MEM30-C: 不要访问已释放的内存 */
/* 非合规： */
char *buffer = malloc(1024);
free(buffer);
strcpy(buffer, "data");  /* UAF漏洞！ */

/* 合规： */
char *buffer = malloc(1024);
if (buffer != NULL) {
    strcpy(buffer, "data");
    /* 使用buffer... */
    free(buffer);
    buffer = NULL;  /* 置空指针防止误用 */
}

/* STR31-C: 保证字符串存储空间足够 */
/* 非合规： */
char dest[5];
strcpy(dest, "Hello World!");  /* 缓冲区溢出！ */

/* 合规： */
char dest[5];
strncpy(dest, "Hello World!", sizeof(dest) - 1);
dest[sizeof(dest) - 1] = '\0';  /* 确保终止 */
