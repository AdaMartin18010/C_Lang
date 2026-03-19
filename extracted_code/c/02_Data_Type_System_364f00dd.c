/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\02_Data_Type_System.md
 * Line: 136
 * Language: c
 * Block ID: 364f00dd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define abs(x) _Generic((x), \
    int: abs_int, \
    long: abs_long, \
    double: abs_double \
)(x)
