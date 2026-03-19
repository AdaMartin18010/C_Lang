/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\06_Modern_C_C23.md
 * Line: 106
 * Language: c
 * Block ID: 5ffdae00
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C89-C17: 自动存储期 (几乎不用)
auto int x = 42;

// C23: 类型推导
auto y = 42;        // y推导为int
auto z = 3.14;      // z推导为double
auto s = "hello";   // s推导为const char*

// 配合typeof使用
typedef struct { int x, y; } Point;
auto p = (Point){10, 20};  // p推导为Point
