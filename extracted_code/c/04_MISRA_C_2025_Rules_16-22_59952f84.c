/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 558
 * Language: c
 * Block ID: 59952f84
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 检查C23支持 */
#if __STDC_VERSION__ >= 202311L
    /* C23代码 */
    #define nullptr ((void*)0)
#else
    /* C17兼容代码 */
    #define nullptr NULL
#endif
