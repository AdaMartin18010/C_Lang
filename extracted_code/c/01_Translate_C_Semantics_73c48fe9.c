/*
 * Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
 * Line: 540
 * Language: c
 * Block ID: 73c48fe9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码
struct __attribute__((packed)) PackedHeader {
    uint8_t type;
    uint16_t length;
    uint32_t checksum;
};

#pragma pack(push, 1)
struct PackedData {
    char id;
    int value;
    double data;
};
#pragma pack(pop)
