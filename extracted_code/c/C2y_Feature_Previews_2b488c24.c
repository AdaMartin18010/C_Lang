/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 874
 * Language: c
 * Block ID: 2b488c24
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============ C++ auto ============
// C++11及以后

// auto保留引用?
int x = 42;
int& ref = x;
auto a = ref;      // int (复制)
auto& b = ref;     // int& (引用)

// auto与const
const int c = 42;
auto d = c;        // int (const被移除)
const auto e = c;  // const int

// auto返回类型 (C++14)
auto func() {      // 推导返回类型
    return 42;
}

// ============ C2y auto (提案) ============
// C2y没有引用，所以更简单

// 主要差异:
// 1. C2y auto不能用于函数返回类型
// 2. C2y auto不能用于参数(无C++概念)
// 3. C2y数组推导为指针(与C一致)
// 4. C2y函数推导为函数指针

// 共同点:
// 1. 顶层cv限定符被移除
// 2. 底层cv限定符保留
// 3. 编译期类型推导
