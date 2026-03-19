/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\03_Operators_Expressions.md
 * Line: 235
 * Language: c
 * Block ID: 4c5cbb0f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 整型提升
char c = 'A';
short s = 100;
int result = c + s;  // c和s都提升为int

// 混合类型运算
double d = 3.14;
int i = 42;
double r = d + i;    // i转换为double

// 无符号与有符号
unsigned int u = 10;
int si = -5;
// u + si: si转换为unsigned int，变成很大的数！

// 安全的类型转换
define SAFE_CAST(to_type, value) ((to_type)(value))

// 显式检查
int add_safe(int a, int b) {
    if (b > 0 && a > INT_MAX - b) {
        // 溢出
        return INT_MAX;
    }
    if (b < 0 && a < INT_MIN - b) {
        // 下溢
        return INT_MIN;
    }
    return a + b;
}
