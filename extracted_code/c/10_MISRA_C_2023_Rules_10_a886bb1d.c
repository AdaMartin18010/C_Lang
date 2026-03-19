/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\10_MISRA_C_2023_Rules_10.md
 * Line: 387
 * Language: c
 * Block ID: a886bb1d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 清晰的存储类 */

/* header.h */
#ifndef HEADER_H
#define HEADER_H

/* 外部声明在头文件中 */
extern int g_system_status;
extern void system_init(void);

#endif

/* module.c */
#include "header.h"

/* 定义（不带extern）*/
int g_system_status = 0;

/* 内部函数 - static */
static void helper(void)
{
    /* ... */
}

void system_init(void)
{
    g_system_status = 1;
    helper();
}

/* ✅ 合规 - 单文件static */
static int s_instance_count = 0;  /* 文件私有 */

void create_instance(void)
{
    s_instance_count++;
}

void destroy_instance(void)
{
    s_instance_count--;
}

int get_instance_count(void)
{
    return s_instance_count;
}
