/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\03_Portability.md
 * Line: 154
 * Language: c
 * Block ID: e603a8cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 使用标准类型
#include <stddef.h>   // size_t, ptrdiff_t, NULL
#include <stdint.h>   // int32_t, uint64_t等
#include <stdbool.h>  // bool, true, false (C99+)

// ✅ 避免依赖char的符号性
// char可能是signed或unsigned，由实现定义
// 明确指定：
signed char  signed_byte;    // 有明确符号
unsigned char unsigned_byte; // 无符号

// ✅ 使用limits.h而非硬编码
#include <limits.h>
#if INT_MAX >= 2147483647
    // 确保至少32位
#endif

// ✅ 可移植的文件路径
#ifdef _WIN32
    #define PATH_SEP "\\"
#else
    #define PATH_SEP "/"
#endif

// ✅ 可移植的导出声明
#ifdef _WIN32
    #ifdef BUILDING_DLL
        #define EXPORT __declspec(dllexport)
    #else
        #define EXPORT __declspec(dllimport)
    #endif
#else
    #define EXPORT __attribute__((visibility("default")))
#endif
