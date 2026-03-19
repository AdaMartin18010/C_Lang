/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\02_Data_Type_System.md
 * Line: 514
 * Language: c
 * Block ID: 61fdc302
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// UNSAFE: 赋值给较窄类型导致截断
long long big = 10000000000LL;  // 需要超过32位
int small = big;  // 截断！值变为 1410065408 (在32位系统)

// UNSAFE: 浮点转整数截断小数（向零舍入）
double pi = 3.14159;
int approx = pi;  // 3，不是4

// UNSAFE: 溢出
int max = INT_MAX;
int overflow = max + 1;  // 未定义行为(有符号溢出)
