/*
 * Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
 * Line: 910
 * Language: c
 * Block ID: 6e0b4f8d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码
struct AlignedStruct {
    char c;
    _Alignas(16) int x;
    double d;
};

struct __attribute__((aligned(32))) CacheLine {
    char data[64];
};
