/*
 * Auto-generated from: 07_Modern_Toolchain\05_Code_Quality_Toolchain\02_Static_Analysis_Deep_Dive.md
 * Line: 316
 * Language: c
 * Block ID: 80d96193
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 变量在程序点是否还会被使用
int live_example() {
    int a = 1;      // a是活跃的
    int b = a;      // a是活跃的, b变为活跃
    int c = b + 1;  // b是活跃的, c变为活跃
    // a在这里不再活跃(可以被优化掉)
    return c;       // 只有c是活跃的
}
