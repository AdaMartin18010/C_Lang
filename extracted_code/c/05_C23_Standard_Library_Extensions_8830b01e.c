/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\05_C23_Standard_Library_Extensions.md
 * Line: 1841
 * Language: c
 * Block ID: 8830b01e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11风格（仍然有效）
_Alignas(16) char buffer[256];
size_t a = _Alignof(double);

// C23新增：alignas 和 alignof 作为关键字
// 无需包含头文件即可使用
alignas(16) char buffer2[256];
size_t a2 = alignof(double);

// 头文件现在主要提供兼容性宏
#ifndef __STDC_VERSION__
    #define alignas _Alignas
    #define alignof _Alignof
#endif
