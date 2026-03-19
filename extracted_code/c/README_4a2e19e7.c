/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\README.md
 * Line: 363
 * Language: c
 * Block ID: 4a2e19e7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 指定初始化器增强
struct Config {
    int timeout;
    int retries;
    const char *name;
};

struct Config cfg = {
    .timeout = 30,
    .retries = 3,
    .name = "default",
};

// 数组指定初始化 (C23 增强)
int array[10] = {
    [0] = 1,
    [5 ... 8] = 42,  // 范围初始化
    [9] = 99
};

// typeof 运算符 (C23)
typeof(int) x = 10;               // x 是 int
typeof(x) y = 20;                 // y 与 x 同类型
typeof(int[5]) arr;               // arr 是 int[5]

// auto 类型推导 (C23)
// auto i = 42;         // i 推导为 int
// auto s = "hello";    // s 推导为 char*
// auto f = 3.14f;      // f 推导为 float

// constexpr (C23)
// constexpr int SIZE = 100;
// constexpr int factorial(int n) {
//     return n <= 1 ? 1 : n * factorial(n - 1);
// }
