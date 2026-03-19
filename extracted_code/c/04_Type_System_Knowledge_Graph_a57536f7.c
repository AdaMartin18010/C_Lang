/*
 * Auto-generated from: 06_Thinking_Representation\07_Knowledge_Graph\04_Type_System_Knowledge_Graph.md
 * Line: 294
 * Language: c
 * Block ID: a57536f7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

double d = 3.14;
int i = (int)d;           // C 风格

// C11 泛型选择（类型安全的"转换"）
#define ABS(x) _Generic((x), \
    int: abs, \
    long: labs, \
    long long: llabs, \
    double: fabs \
)(x)
