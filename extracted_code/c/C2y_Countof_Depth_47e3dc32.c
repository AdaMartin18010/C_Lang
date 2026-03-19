/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1887
 * Language: c
 * Block ID: 47e3dc32
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 在GCC完全支持之前，使用以下方案

#if defined(__GNUC__) && !defined(__clang__)
    // GCC特定处理
    #define COUNTOF_COMPAT(arr) (sizeof(arr) / sizeof((arr)[0]))
#else
    #define COUNTOF_COMPAT(arr) _Countof(arr)
#endif
