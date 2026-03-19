/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\02_Pointer_Deep_Dive.md
 * Line: 824
 * Language: c
 * Block ID: 7ce1f508
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 指针大小变化的影响
struct Node32 {
    void *next;     // 4字节
    int data;       // 4字节
};                  // 共8字节

struct Node64 {
    void *next;     // 8字节
    int data;       // 4字节
    int padding;    // 4字节（对齐填充）
};                  // 共16字节！
