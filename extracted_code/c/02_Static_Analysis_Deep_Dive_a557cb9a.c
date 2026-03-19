/*
 * Auto-generated from: 07_Modern_Toolchain\05_Code_Quality_Toolchain\02_Static_Analysis_Deep_Dive.md
 * Line: 301
 * Language: c
 * Block ID: a557cb9a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 每个变量定义能到达哪些使用点
int example(int a) {
    int x;          // D1: x = undefined
    if (a > 0) {
        x = 1;      // D2: x = 1
    } else {
        x = 2;      // D3: x = 2
    }
    return x;       // x的可能值: {D2, D3}
}
