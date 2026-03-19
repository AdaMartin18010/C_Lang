/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\02_C17_C23_Features.md
 * Line: 113
 * Language: c
 * Block ID: 98d1f13c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 编译期计算
constexpr int square(int x) {
    return x * x;
}

// 编译期结果
int arr[square(5)];  // int arr[25];

// 与const不同
const int c = 5;           // 只读变量
constexpr int ce = 5;      // 编译期常量

// 可用于switch case
switch (x) {
    case square(3):  // case 9:
        // ...
}

// 静态初始化
static constexpr double PI = 3.14159265359;
