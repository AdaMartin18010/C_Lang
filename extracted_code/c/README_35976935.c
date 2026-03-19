/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\README.md
 * Line: 116
 * Language: c
 * Block ID: 35976935
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 1. 开发环境配置 */
// 推荐工具链
// • GCC (MinGW/Cygwin on Windows, built-in on Linux/Mac)
// • VS Code / CLion / Vim
// • GDB 调试器
// • Make 构建工具

/* 2. Hello World 程序结构 */
#include <stdio.h>    // 头文件包含

// 入口函数
int main(void) {
    // 语句以分号结束
    printf("Hello, World!\n");
    return 0;         // 返回值
}

/* 3. 变量与数据类型 */
void data_types_demo() {
    // 整数类型
    char c = 'A';                 // 1字节
    short s = 100;                // 2字节
    int i = 10000;                // 4字节
    long l = 1000000L;            // 4/8字节
    long long ll = 10000000000LL; // 8字节

    // 无符号类型
    unsigned int ui = 100U;

    // 浮点类型
    float f = 3.14f;              // 4字节
    double d = 3.14159;           // 8字节
    long double ld = 3.14159L;    // 8+字节

    // 类型大小查询
    printf("sizeof(int) = %zu\n", sizeof(int));
}

/* 4. 运算符 */
void operators_demo() {
    int a = 10, b = 3;

    // 算术运算
    int sum = a + b;      // 13
    int diff = a - b;     // 7
    int prod = a * b;     // 30
    int quot = a / b;     // 3 (整数除法)
    int rem = a % b;      // 1 (取模)

    // 关系运算
    int eq = (a == b);    // 0 (false)
    int gt = (a > b);     // 1 (true)

    // 逻辑运算
    int and = (a > 0) && (b > 0);  // 1
    int or = (a > 0) || (b < 0);   // 1
    int not = !(a > b);            // 0

    // 位运算
    int band = a & b;     // 按位与
    int bor = a | b;      // 按位或
    int bxor = a ^ b;     // 按位异或
    int bnot = ~a;        // 按位取反
    int lsh = a << 2;     // 左移
    int rsh = a >> 1;     // 右移
}
