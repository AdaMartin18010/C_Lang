/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
 * Line: 1002
 * Language: c
 * Block ID: b1d2c749
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 整数↔指针转换
#include <stdint.h>
#include <assert.h>

void int_to_pointer_conversions(void) {
    int x = 42;
    int* p = &x;

    // 指针 → 整数（可能丢失来源）
    uintptr_t addr = (uintptr_t)p;

    // 整数 → 指针（尝试恢复来源）
    int* q = (int*)addr;

    // 如果实现使用 PVI，q 与 p 来源相同
    // 如果实现使用 PNVI，q 可能根据地址恢复来源
    assert(*q == 42);  // 通常工作，但理论上依赖实现

    // 危险的转换：地址计算
    uintptr_t addr2 = addr + sizeof(int);
    int* r = (int*)addr2;  // 可能指向 x 之后，one-past-end
    // 解引用 r 是未定义行为！

    (void)r;
}

// 内存标签/着色（使用高位）
#ifdef __aarch64__
void memory_tagging(void) {
    int* p = malloc(sizeof(int));

    // ARM MTE: 使用指针高位作为标签
    // 需要特殊指令处理

    free(p);
}
#endif
