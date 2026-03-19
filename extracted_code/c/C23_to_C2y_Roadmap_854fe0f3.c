/*
 * Auto-generated from: 00_VERSION_TRACKING\C23_to_C2y_Roadmap.md
 * Line: 317
 * Language: c
 * Block ID: 854fe0f3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 迁移检查清单:

// 1. 替换NULL使用nullptr
// 旧:
int *p = NULL;
if (p != NULL) { ... }

// 新:
int *p = nullptr;
if (p != nullptr) { ... }  // 更清晰类型安全

// 2. 使用constexpr替代部分宏
// 旧:
#define BUFFER_SIZE 1024
#define SQUARE(x) ((x) * (x))

// 新:
constexpr int BUFFER_SIZE = 1024;
constexpr int square(int x) { return x * x; }  // C23 constexpr函数

// 3. 使用typeof替代typeof (标准化)
// 旧 (GNU):
typedef __typeof__(int*) int_ptr;

// 新 (标准):
typedef typeof(int*) int_ptr;
typedef typeof_unqual(const int) plain_int;  // 去除cv限定

// 4. 使用标准属性
// 旧 (GCC特定):
__attribute__((noreturn)) void die(void);
__attribute__((deprecated)) void old(void);

// 新 (标准):
[[noreturn]] void die(void);
[[deprecated]] void old(void);

// 5. 使用_BitInt替代非标准扩展
// 旧 (Clang扩展):
int __attribute__((bit_width(17))) x;

// 新 (标准):
_BitInt(17) x;

// 6. 使用#embed替代hex数组
// 旧:
static const uint8_t icon_data[] = {
    0x89, 0x50, 0x4E, 0x47, ...  // 手动转换
};

// 新:
static const uint8_t icon_data[] = {
    #embed "icon.png"
};

// 7. 使用新库函数
// 旧:
int bits = __builtin_popcount(x);

// 新:
#include <stdbit.h>
int bits = stdc_count_ones(x);

// 8. 安全检查算术
// 旧:
int result = a + b;
if (a > 0 && b > 0 && result < 0) { /* 溢出 */ }

// 新:
#include <stdckdint.h>
bool overflow;
int result = ckd_add(&overflow, a, b);
if (overflow) { /* 处理溢出 */ }
