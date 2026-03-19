/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\02_C17_C23_Features.md
 * Line: 308
 * Language: c
 * Block ID: e4a16183
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 auto
auto x = 3.14f;  // float

// C++ auto 会推导出 float
// 但C23和C++在某些上下文中行为不同

// C23: auto不能用于函数参数
// void func(auto x);  // 错误！

// C++: auto可用于模板函数参数
// template<typename T> void func(T x);
