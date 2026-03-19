/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\01_Nullptr_vs_Optional_Pointer.md
 * Line: 463
 * Language: c
 * Block ID: e46d1e01
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 空指针解引用防护
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

// 机制 1: 断言 (运行时)
void safe_dereference_with_assert(int* ptr) {
    // 开发者负责添加检查
    assert(ptr != nullptr);

    // 现在可以安全解引用
    int value = *ptr;
    (void)value;
}

// 机制 2: 防御式编程模式
void defensive_pattern(int* ptr) {
    // 早期返回模式
    if (ptr == nullptr) {
        // 处理错误
        return;
    }

    // 后续代码可以假定 ptr 非空
    *ptr = 42;
}

// C23 防护的局限性
void c23_limitations(void) {
    int* p = nullptr;

    // 以下代码编译通过，运行时崩溃
    // *p = 42;  // SIGSEGV!

    // 检查可以被遗忘
    int* r = get_maybe_null();
    // 忘记检查，直接解引用
    // *r = 42;  // 潜在崩溃
}

int* get_maybe_null(void) { return nullptr; }
