/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 810
 * Language: c
 * Block ID: 8959f900
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例1: 基础推导
void basic_inference(void) {
    auto i = 42;           // int
    auto d = 3.14;         // double
    auto f = 3.14f;        // float
    auto c = 'A';          // int (C中字符常量是int)
    auto s = "hello";      // const char*

    int arr[10];
    auto a = arr;          // int* (数组退化为指针)

    void func(void);
    auto fp = func;        // void (*)(void)
}

// 示例2: 复杂表达式
void complex_expressions(void) {
    int x = 10;
    const int* ptr = &x;
    auto p = ptr;          // const int* (const在底层，保留)

    int* const cptr = &x;
    auto cp = cptr;        // int* (顶层const被移除)

    auto sum = x + 3.14;   // double (通常算术转换)
}

// 示例3: 与函数返回结合
auto get_value(void) {     // ❌ C2y不支持auto返回类型
    return 42;
}

// 正确做法: 显式返回类型
int get_value_explicit(void) {
    auto result = compute();  // 函数体内使用auto
    return result;
}

// 示例4: 迭代变量
void iteration(void) {
    int data[] = {1, 2, 3, 4, 5};

    for (auto i = 0; i < _Countof(data); i++) {
        // i是int
    }

    // 遍历指针
    for (auto p = data; p < data + _Countof(data); p++) {
        // p是int*
    }
}

// 示例5: 与宏结合
#define DECL_VAR(name, init) auto name = init

void macro_usage(void) {
    DECL_VAR(x, 42);       // auto x = 42
    DECL_VAR(y, 3.14);     // auto y = 3.14
}
