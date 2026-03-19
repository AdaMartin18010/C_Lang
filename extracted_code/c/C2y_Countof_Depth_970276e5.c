/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1344
 * Language: c
 * Block ID: 970276e5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误案例1：指针误用
void process(int arr[]) {
    // 错误：使用sizeof计算元素数
    size_t n = sizeof(arr) / sizeof(arr[0]);  // 返回指针大小 / int大小
    // 在64位系统上可能是 8/4 = 2，完全错误！

    // _Countof(arr);  // 编译错误，正确发现问题
}

// 错误案例2：优先级问题
#define BAD_COUNT(arr) sizeof(arr) / sizeof(arr[0])  // 缺少外层括号

int a[10];
size_t n = BAD_COUNT(a) + 1;  // 展开: sizeof(a) / sizeof(a[0]) + 1
                              // 实际: sizeof(a) / (sizeof(a[0]) + 1)
                              // 结果错误！

// 正确写法需要复杂宏
#define GOOD_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

// _Countof不需要宏，直接使用
size_t m = _Countof(a) + 1;  // 正确
