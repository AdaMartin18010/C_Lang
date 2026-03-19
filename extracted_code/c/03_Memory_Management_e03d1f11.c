/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\03_Memory_Management.md
 * Line: 68
 * Language: c
 * Block ID: e03d1f11
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 代码段 - 存储在 .text
int add(int a, int b) {
    return a + b;
}

// 数据段 - .data (已初始化)
int global_init = 42;

// BSS段 - .bss (未初始化)
int global_uninit;

static int static_var = 100;  // .data
