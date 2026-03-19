/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\01_Language_Extensions.md
 * Line: 84
 * Language: c
 * Block ID: fdaff50d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 对齐
typedef int aligned_int __attribute__((aligned(64)));

// 打包结构体（无填充）
struct __attribute__((packed)) Header {
    uint8_t type;
    uint32_t length;
};

// 透明联合体
typedef union {
    int i;
    float f;
} value __attribute__((transparent_union));

// 设计ated初始化
struct Point { int x, y; };
struct Point p = { .x = 10, .y = 20 };
