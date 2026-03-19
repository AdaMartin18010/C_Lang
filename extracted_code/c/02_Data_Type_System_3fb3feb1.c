/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\02_Data_Type_System.md
 * Line: 918
 * Language: c
 * Block ID: 3fb3feb1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ UNSAFE: 隐式转换陷阱
double calc(int a, int b) {
    return a / b;  // 整数除法后再转double！
}

// ✅ SAFE: 显式转换
double calc_safe(int a, int b) {
    if (b == 0) return 0.0;  // 除零检查
    return (double)a / (double)b;  // 浮点除法
}
