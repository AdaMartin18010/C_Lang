/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\06_Modern_C_C23.md
 * Line: 59
 * Language: c
 * Block ID: 9d4ad751
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 编译期常量
constexpr int max_size = 100;
constexpr int doubled = max_size * 2;

// 编译期函数 (简化版)
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

constexpr int fact5 = factorial(5);  // 编译期计算: 120

// 用于数组大小
int buffer[doubled];  // 合法，doubled是编译期常量
