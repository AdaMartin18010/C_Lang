/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_C23_Guidance.md
 * Line: 136
 * Language: c
 * Block ID: e137ee85
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 迁移步骤 [待官方确认] */

/* 1. 创建兼容宏 */
/* common.h */
#ifndef COMPAT_H
#define COMPAT_H

#if __STDC_VERSION__ < 202311L
    #ifndef nullptr
        #define nullptr NULL
    #endif
#endif

#endif

/* 2. 批量替换 NULL → nullptr */
/* 对于指针上下文 */
int *p = nullptr;  /* 替换 int *p = NULL; */

/* 3. 测试验证 */
/* 确保所有使用nullptr的地方都是指针上下文 */
