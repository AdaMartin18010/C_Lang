/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\02_C17_C23_Features.md
 * Line: 138
 * Language: c
 * Block ID: f93056de
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: auto类型推导（类似C++11）
auto i = 42;           // int
auto d = 3.14;         // double
auto s = "hello";      // const char*
auto p = &i;           // int*

// 复合类型
auto arr = (int[]){1, 2, 3};  // int[3]

// 不能用于函数参数（C23限制）
// void func(auto x);  // 错误！

// 与typeof结合
auto x = some_function();
typeof(x) y;  // 相同类型
