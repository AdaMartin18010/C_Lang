/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\01_Standard_IO\README.md
 * Line: 58
 * Language: c
 * Block ID: dc244926
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* FILE 结构体 - 具体实现因平台而异 */
typedef struct _IO_FILE FILE;

/* 标准流 */
extern FILE *stdin;   /* 标准输入 */
extern FILE *stdout;  /* 标准输出 */
extern FILE *stderr;  /* 标准错误输出 */

/* 文件位置类型 */
typedef long fpos_t;  /* C89/C99 */
/* typedef _G_fpos64_t fpos_t; */  /* 64位扩展 */
