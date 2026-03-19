/*
 * Auto-generated from: 12_Practice_Exercises\06_Performance_Optimization.md
 * Line: 325
 * Language: c
 * Block ID: 5090fd18
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 内存布局差 (填充多)
typedef struct {
    char a;      // 1 byte + 7 padding
    double b;    // 8 bytes
    char c;      // 1 byte + 7 padding
    double d;    // 8 bytes
} BadLayout;     // 32 bytes

// 内存布局好 (紧凑)
typedef struct {
    double b;    // 8 bytes
    double d;    // 8 bytes
    char a;      // 1 byte
    char c;      // 1 byte + 6 padding
} GoodLayout;    // 24 bytes
