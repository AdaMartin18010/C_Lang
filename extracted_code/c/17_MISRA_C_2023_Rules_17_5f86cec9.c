/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\17_MISRA_C_2023_Rules_17.md
 * Line: 256
 * Language: c
 * Block ID: 5f86cec9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ GCC扩展 - 透明联合 */
typedef union {
    int *i;
    float *f;
} Transparent __attribute__((transparent_union));

void func(Transparent t);  /* 可接受int*或float*，危险 */
