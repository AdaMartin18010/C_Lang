/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\14_MISRA_C_2023_Rules_14.md
 * Line: 239
 * Language: c
 * Block ID: c4aefab0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 问题 - 跨文件初始化顺序不确定 */
/* file1.c */
int get_config_value(void);
int global_var = get_config_value();  /* 动态初始化 */

/* file2.c */
extern int global_var;
int another_var = global_var + 1;  /* 依赖global_var，但顺序不确定 */
