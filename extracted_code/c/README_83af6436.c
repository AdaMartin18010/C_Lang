/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\CERT_C_2024\README.md
 * Line: 87
 * Language: c
 * Block ID: 83af6436
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 constexpr - 编译时常量
constexpr int MAX_BUFFER_SIZE = 1024;  // 编译期确定，避免运行时错误

// C23 nullptr 替代 NULL
void* p = nullptr;  // 类型安全的空指针

// C23 属性语法标准化
[[nodiscard]] int critical_operation(void);  // 强制检查返回值
[[maybe_unused]] int debug_var = 0;          // 显式标记可能未使用

// C23 typeof 运算符
typeof(int) x;  // 类型推导，减少类型错误
typeof_unqual(const int) y;  // 移除类型限定符
