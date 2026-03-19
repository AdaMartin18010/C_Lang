/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\README.md
 * Line: 268
 * Language: c
 * Block ID: a8edfa3e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C23: 使用宏和_Generic */
#define MAX(a, b) _Generic((a), \
    int: max_int, \
    float: max_float, \
    double: max_double \
)(a, b)

static inline int max_int(int a, int b) {
    return a > b ? a : b;
}

int m1 = MAX(10, 20);
double m2 = MAX(3.14, 2.71);
