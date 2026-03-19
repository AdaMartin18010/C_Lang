/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\03_C23_Core_Features.md
 * Line: 63
 * Language: c
 * Block ID: da88a94d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 和 C++23 共享的特性
constexpr int size = 100;           // 两者都支持
nullptr_t p = nullptr;              // 两者语义一致
auto x = 5;                         // C23 的 auto 是类型推导
[[nodiscard]] int func(void);       // 属性语法一致
[[maybe_unused]] int unused;        // 属性语法一致
[[deprecated]] void old_func(void); // 属性语法一致
