/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\01_Nullptr_vs_Optional_Pointer.md
 * Line: 156
 * Language: c
 * Block ID: 745642c9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 - nullptr 解决上述问题
#include <stddef.h>  // 或直接使用

void demo_nullptr(void) {
    int* p1 = nullptr;        // OK

    // 改进 1: 不能隐式转换为整数
    // int x = nullptr;       // ERROR: 禁止转换

    // 改进 2: 类型明确
    nullptr_t nt = nullptr;   // OK，有专门的类型

    // 改进 3: sizeof 确定
    // sizeof(nullptr_t) == sizeof(void*)

    // 但: 仍然需要手动检查
    // *p1;  // 如果 p1 是 nullptr，未定义行为！
}
