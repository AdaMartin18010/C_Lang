/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 1816
 * Language: c
 * Block ID: a62167ad
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 原始代码
#define DECLARE_GENERIC_MAX(TYPE) \
    TYPE max_##TYPE(TYPE a, TYPE b) { \
        return a > b ? a : b; \
    }

DECLARE_GENERIC_MAX(int)
DECLARE_GENERIC_MAX(double)

#define GENERIC_MAX(a, b) _Generic((a), \
    int: max_int, \
    double: max_double \
)(a, b)
