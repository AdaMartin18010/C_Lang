/*
 * Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
 * Line: 613
 * Language: c
 * Block ID: ef0234e2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码
union Data {
    int i;
    float f;
    char bytes[4];
};

union AlignedData {
    char c;
    int i;
} __attribute__((aligned(8)));
