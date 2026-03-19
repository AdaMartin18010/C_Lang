/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\22_MISRA_C_2023_Rules_22.md
 * Line: 150
 * Language: c
 * Block ID: 562093bb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 文件头注释 */
/**
 * @file module.c
 * @brief Brief description of the module
 *
 * Detailed description of what this module does,
 * its responsibilities, and how to use it.
 *
 * @copyright Copyright 2026
 * @license MIT
 */

/* ✅ 函数注释 */
/**
 * @brief Calculate the factorial of n
 *
 * @param n The input value (0 <= n <= 12)
 * @return The factorial of n, or -1 if error
 *
 * @note This function uses iterative algorithm
 *       to avoid stack overflow
 */
int factorial(int n);

/* ✅ 代码块注释 */
/* Validate input parameters */
if (ptr == NULL) {
    return ERROR_NULL_POINTER;
}

/* Process each element in the array */
for (size_t i = 0; i < count; i++) {
    /* Skip disabled items */
    if (!items[i].enabled) {
        continue;
    }

    /* Apply transformation */
    process(&items[i]);
}

/* ❌ 避免 - 无关注释 */
i++;  /* Increment i */

/* ❌ 避免 - 过时注释 */
/* Calculate average of 5 values */  /* 实际处理10个值 */
