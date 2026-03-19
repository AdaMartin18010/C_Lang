/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1588
 * Language: c
 * Block ID: e5a661cd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// array_utils.h
#pragma once

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
    // C23支持_Countof
    #define ARRAY_SIZE(arr) _Countof(arr)
#else
    // 回退到传统方式
    #define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif
