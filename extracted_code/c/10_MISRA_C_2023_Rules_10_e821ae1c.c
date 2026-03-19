/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\10_MISRA_C_2023_Rules_10.md
 * Line: 633
 * Language: c
 * Block ID: e821ae1c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 头文件声明，源文件定义 */
/* config.h */
#ifndef CONFIG_H
#define CONFIG_H

extern int g_max_connections;
extern float g_timeout_seconds;

#endif

/* config.c */
#include "config.h"

int g_max_connections = 100;      /* 唯一定义 */
float g_timeout_seconds = 30.0f;  /* 唯一定义 */

/* ✅ 合规 - static内联定义（如果必须）*/
/* header.h */
#ifndef HEADER_H
#define HEADER_H

static inline int get_version(void)
{
    return 1;  /* static inline可出现在多个翻译单元 */
}

#endif

/* ✅ 合规 - 使用extern inline */
extern inline int fast_max(int a, int b)
{
    return (a > b) ? a : b;
}
