/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\04_Functions_Scope.md
 * Line: 195
 * Language: c
 * Block ID: 575ac6c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// file1.c
int external_var = 100;  // 外部链接
default int default_var = 200;  // 同external
static int internal_var = 300;  // 内部链接

// file2.c
extern int external_var;  // 引用file1.c中的变量
// extern int internal_var;  // 错误！internal_var是内部链接
