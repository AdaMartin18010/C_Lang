/*
 * Auto-generated from: 07_Modern_Toolchain\12_Modern_C_Standards\01_C11_to_C23.md
 * Line: 98
 * Language: c
 * Block ID: 3351c793
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* typeof: 类型推导 */
typeof(int) x;           /* x是int */
typeof(x) y;             /* y也是int */
#define swap(a, b) do { \
    typeof(a) tmp = (a); \
    (a) = (b); \
    (b) = tmp; \
} while(0)

/* nullptr */
void* p = nullptr;  /* 比NULL类型安全 */

/* constexpr */
constexpr int size = 100;  /* 编译时常量 */

/* 属性 */
[[nodiscard]] int must_use_result(void);
[[maybe_unused]] void unused_func(void);
[[noreturn]] void abort(void);
[[deprecated("Use new_func instead")]] void old_func(void);

/* 二进制字面量 */
int flags = 0b1010'1100;  /* 数字分隔符 */

/* 变长数组可选 (VLA) */
/* C11: VLA是强制要求 */
/* C23: VLA是可选特性，可用__STDC_NO_VLA__检测 */

/* 标准属性 */
[[unsequenced]]    /* 函数无副作用，可重排序 */
[[reproducible]]   /* 函数纯计算，相同输入必同输出 */

/* 改进的枚举 */
enum Color : uint8_t { RED, GREEN, BLUE };  /* 指定底层类型 */

/* 隐式int的移除 */
/* C23之前: main() {} 允许 */
/* C23: 必须显式返回类型 */

/* 新库函数 */
#include <stdckdint.h>
bool overflow = ckd_add(&result, a, b);  /* 检查溢出加法 */
