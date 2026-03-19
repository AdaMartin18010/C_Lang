/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\01_Nullptr_vs_Optional_Pointer.md
 * Line: 220
 * Language: c
 * Block ID: 4f3a4223
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 nullptr 完整语法示例
#include <stddef.h>
#include <stdbool.h>

// 基本用法
void basic_usage(void) {
    int* ip = nullptr;            // OK
    char* cp = nullptr;           // OK
    void* vp = nullptr;           // OK
    int (*fp)(int) = nullptr;     // 函数指针

    // 类型检查
    static_assert(sizeof(nullptr_t) == sizeof(void*), "size mismatch");

    // 布尔上下文
    if (nullptr) {                // false
        // 永远不会执行
    }

    bool b = nullptr;             // OK: 转换为 false
    _Bool bb = nullptr;           // OK
}

// 比较语义
void comparison_semantics(void) {
    int* p = nullptr;

    // 相等比较
    bool eq1 = (p == nullptr);    // true
    bool eq2 = (nullptr == p);    // true (对称)

    // 与 NULL 比较 (向后兼容)
    bool eq3 = (p == NULL);       // true (如果 NULL 是空指针常量)

    // 不等比较
    bool ne = (p != nullptr);     // false

    // 关系比较 - 未定义或实现定义
    // bool rel = (p < nullptr);  // 禁止或 UB
}
