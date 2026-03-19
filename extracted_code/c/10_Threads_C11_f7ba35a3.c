/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\10_Threads_C11.md
 * Line: 110
 * Language: c
 * Block ID: f7ba35a3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 检测C11线程支持 */
#if defined(__STDC_NO_THREADS__) && __STDC_NO_THREADS__
    #error "Compiler does not support C11 threads"
#endif

/* 各编译器支持情况 */
#if defined(__GLIBC__) && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 28
    /* glibc 2.28+ 原生支持 */
#elif defined(__clang__)
    /* Clang 3.6+ 支持，但某些平台需要链接 -lpthread */
#elif defined(_MSC_VER) && _MSC_VER >= 1900
    /* MSVC 2015+ 部分支持，2019+ 完整支持 */
#endif
