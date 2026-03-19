/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1742
 * Language: c
 * Block ID: dfabce8d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// feature_detect.h
#ifndef FEATURE_DETECT_H
#define FEATURE_DETECT_H

// 检测C23支持
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
    #define HAS_C23 1
#else
    #define HAS_C23 0
#endif

// 检测_Countof支持
#if HAS_C23
    #define HAS_COUNTOF 1
#elif defined(__has_feature)
    #if __has_feature(c_countof)
        #define HAS_COUNTOF 1
    #endif
#endif

#ifndef HAS_COUNTOF
    #define HAS_COUNTOF 0
#endif

// 宏定义
#if HAS_COUNTOF
    #define COUNTOF _Countof
#else
    #define COUNTOF(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#endif // FEATURE_DETECT_H
