/*
 * Auto-generated from: 06_Thinking_Representation\08_Index\04_Modern_C_Quick_Reference.md
 * Line: 89
 * Language: c
 * Block ID: 2f81779b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define abs(x) _Generic((x), \
    int: abs_int, \
    long: abs_long, \
    float: fabsf, \
    double: fabs \
)(x)
