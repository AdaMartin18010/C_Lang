/*
 * Auto-generated from: 07_Modern_Toolchain\05_Code_Quality_Toolchain\02_Static_Analysis_Deep_Dive.md
 * Line: 366
 * Language: c
 * Block ID: dad05c24
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 工具如何检测这个问题
int risky_calc(int a, int b) {
    int c = a - b;
    return 100 / c;  // 警告: c可能为0
}

// 符号执行发现:
// 当 a == b 时，c == 0，触发除零错误
