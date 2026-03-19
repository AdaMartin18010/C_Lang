/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\01_C11_Features.md
 * Line: 296
 * Language: c
 * Block ID: f77be390
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C17 (修复C11缺陷)
// 主要是bug修复，无新特性

// C23 新特性（部分）
#if __STDC_VERSION__ >= 202311L

// 1. nullptr（类型安全的空指针）
int *p = nullptr;  // 不再是0或(void*)0

// 2. typeof（类型推导）
typof(int) x = 5;  // x是int
auto y = 10;       // 等价，y是int

// 3. 属性简化
[[nodiscard]] int important_func(void);
[[maybe_unused]] int unused_var;

// 4. 二进制常量
int flags = 0b1010'1100;  // 单引号作为分隔符

// 5. #embed 嵌入二进制文件
// const unsigned char icon[] = {
// #embed "icon.png"
// };

// 6. constexpr
constexpr int square(int x) { return x * x; }
int arr[square(5)];  // 编译期计算

#endif
