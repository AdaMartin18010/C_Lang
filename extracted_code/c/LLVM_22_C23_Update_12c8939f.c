/*
 * Auto-generated from: 00_VERSION_TRACKING\LLVM_22_C23_Update.md
 * Line: 280
 * Language: c
 * Block ID: 12c8939f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 及之前：某些情况下可能触发 UB
// C2y：保证不会触发 UB

#include <limits.h>

// 安全的编译时断言
static_assert(sizeof(int) >= 4, "int 必须至少 4 字节");

// 涉及可能溢出表达式的断言
static_assert(1 + 1 == 2, "基本算术");  // C2y 保证安全

// 复杂的编译时检查
#define CHECK_ALIGNMENT(type, align) \
    static_assert(_Alignof(type) % (align) == 0, #type " 未对齐到 " #align)

CHECK_ALIGNMENT(double, 8);
