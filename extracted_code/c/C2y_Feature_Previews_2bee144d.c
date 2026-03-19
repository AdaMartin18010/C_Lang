/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 748
 * Language: c
 * Block ID: 2bee144d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============ 传统C89-C23方式 ============
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// 问题1: 宏定义，无类型检查
int* ptr = malloc(100 * sizeof(int));
size_t n = ARRAY_SIZE(ptr);  // 错误! 但编译通过

// 问题2: 重复求值
size_t n = ARRAY_SIZE(get_array());  // get_array()调用两次!

// 问题3: 零大小数组?
struct { int data[0]; } flex;
// ARRAY_SIZE(flex.data) 可能有问题

// ============ C2y _Countof ============
int arr[100];
int* ptr = arr;

_Countof(arr);   // ✅ 100
_Countof(ptr);   // ❌ 编译错误: 不是数组

// 优势:
// 1. 关键字操作符，不是宏
// 2. 编译期类型检查
// 3. 参数只求值一次(如果是VLA)
// 4. 清晰的错误信息
