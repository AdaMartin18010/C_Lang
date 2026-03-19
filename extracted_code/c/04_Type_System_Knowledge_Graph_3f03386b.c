/*
 * Auto-generated from: 06_Thinking_Representation\07_Knowledge_Graph\04_Type_System_Knowledge_Graph.md
 * Line: 309
 * Language: c
 * Block ID: 3f03386b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define TYPE_NAME(x) _Generic((x), \
    int: "int", \
    double: "double", \
    char*: "string", \
    default: "unknown" \
)

printf("Type: %s\n", TYPE_NAME(42));        // "int"
printf("Type: %s\n", TYPE_NAME(3.14));      // "double"
printf("Type: %s\n", TYPE_NAME("hello"));   // "string"
