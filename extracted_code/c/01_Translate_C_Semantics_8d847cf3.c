/*
 * Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
 * Line: 685
 * Language: c
 * Block ID: 8d847cf3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码
struct HardwareRegister {
    unsigned int enable : 1;
    unsigned int mode : 3;
    unsigned int : 4;        // 匿名填充位域
    unsigned int status : 8;
    unsigned int data : 16;
};
