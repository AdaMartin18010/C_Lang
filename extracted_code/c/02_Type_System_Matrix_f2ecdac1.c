/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\02_Type_System_Matrix.md
 * Line: 301
 * Language: c
 * Block ID: f2ecdac1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用_Generic实现类型泛化
#define max(a, b) _Generic((a), \
    int: max_int, \
    double: max_double, \
    default: max_generic \
)(a, b)

int max_int(int a, int b) { return a > b ? a : b; }
double max_double(double a, double b) { return a > b ? a : b; }

// 使用
int i = max(1, 2);        // 调用 max_int
double d = max(1.0, 2.0); // 调用 max_double
