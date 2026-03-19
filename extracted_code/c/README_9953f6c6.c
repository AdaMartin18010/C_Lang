/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\README.md
 * Line: 61
 * Language: c
 * Block ID: 9953f6c6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 检测编译器支持的 C 标准
#if __STDC_VERSION__ >= 202311L
    #define C_STANDARD 23
    #pragma message("Using C23 standard")
#elif __STDC_VERSION__ >= 201710L
    #define C_STANDARD 17
    #pragma message("Using C17 standard")
#elif __STDC_VERSION__ >= 201112L
    #define C_STANDARD 11
    #pragma message("Using C11 standard")
#elif __STDC_VERSION__ >= 199901L
    #define C_STANDARD 99
    #pragma message("Using C99 standard")
#else
    #define C_STANDARD 90
    #pragma message("Using C89/C90 standard")
#endif

// 特性检测宏
#ifdef __STDC_NO_ATOMICS__
    #error "Atomic operations not supported"
#endif

#ifdef __STDC_NO_THREADS__
    #warning "Threads not supported, using fallback"
#endif

#ifdef __STDC_NO_VLA__
    #warning "Variable length arrays not supported"
#endif
