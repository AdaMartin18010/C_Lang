/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\01_Data_Lab.md
 * Line: 56
 * Language: c
 * Block ID: 4c064c55
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 补码表示（32位）
// 正数：原码
// 负数：反码 + 1

// 特殊值
INT_MIN = 0x80000000  // -2147483648
INT_MAX = 0x7FFFFFFF  // 2147483647

// 符号扩展
// 8位扩展到32位
int sign_extend(char x) {
    return (int)x;  // 自动符号扩展
}
