/*
 * Auto-generated from: 17_Ada_SPARK\01_Ada_Basics\01_Syntax_Types.md
 * Line: 272
 * Language: c
 * Block ID: d4cee807
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

union Shape {
    struct { float radius; } circle;
    struct { float w, h; } rect;
};
// C无法防止访问错误的联合成员
