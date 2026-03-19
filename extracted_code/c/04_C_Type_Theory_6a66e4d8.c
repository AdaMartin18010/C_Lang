/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\00_Core_Semantics_Foundations\04_C_Type_Theory.md
 * Line: 427
 * Language: c
 * Block ID: 6a66e4d8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define abs(x) _Generic((x), \
    int: abs_int, \
    double: abs_double \
)(x)
