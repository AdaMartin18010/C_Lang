/*
 * Auto-generated from: 06_Thinking_Representation\08_Index\04_Modern_C_Quick_Reference.md
 * Line: 75
 * Language: c
 * Block ID: 69b8a0a0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

struct {
    int type;
    union {
        struct { int x, y; };      // 匿名结构体
        struct { float r, g, b; }; // 匿名结构体
    };
} color;

color.x = 10;  // 直接访问
