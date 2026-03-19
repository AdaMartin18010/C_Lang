/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
 * Line: 1119
 * Language: c
 * Block ID: 0eed18df
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: 函数指针调用约定

// 普通函数指针 (使用 System V AMD64 ABI)
typedef int (*callback_t)(int, int);

// 显式指定调用约定
#ifdef _WIN32
    typedef int (__attribute__((ms_abi)) *win_callback_t)(int, int);
#else
    typedef int (*callback_t)(int, int);
#endif

// 函数接受回调
void process_with_callback(int* arr, size_t n, callback_t cb) {
    for (size_t i = 0; i < n; i++) {
        arr[i] = cb(arr[i], i);
    }
}

// 函数返回函数指针
callback_t get_callback(int type);

// 函数指针数组
callback_t callback_table[10];
