/*
 * Auto-generated from: 12_Practice_Exercises\10_Debugging_Puzzles.md
 * Line: 429
 * Language: c
 * Block ID: ea0c2c7d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用宏或泛型（C11）
#define print_array(arr, n, fmt) do { \
    for (int i = 0; i < (n); i++) { \
        printf(fmt " ", (arr)[i]); \
    } \
    printf("\n"); \
} while(0)

// 或C11泛型选择
#define print_val(x) _Generic((x), \
    int: print_int, \
    float: print_float, \
    double: print_double \
)(x)
