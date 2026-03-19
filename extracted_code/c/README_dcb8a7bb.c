/*
 * Auto-generated from: 07_Modern_Toolchain\12_Modern_C_Standards\README.md
 * Line: 455
 * Language: c
 * Block ID: dcb8a7bb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 标准版本宏 */
#if __STDC_VERSION__ >= 202311L
    /* C23 */
    #define C23_FEATURE
#elif __STDC_VERSION__ >= 201710L
    /* C17 */
    #define C17_FEATURE
#elif __STDC_VERSION__ >= 201112L
    /* C11 */
    #define C11_FEATURE
#elif __STDC_VERSION__ >= 199901L
    /* C99 */
    #define C99_FEATURE
#else
    /* C89/C90 */
    #define C89_FEATURE
#endif

/* 特性检查 */
#ifdef __STDC_NO_ATOMICS__
    #error "Atomic operations not supported"
#endif

#ifdef __STDC_NO_THREADS__
    #error "Threading not supported"
#endif

#ifdef __STDC_NO_VLA__
    /* VLA 不支持 */
#endif
