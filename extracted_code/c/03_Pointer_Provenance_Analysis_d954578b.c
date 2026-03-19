/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
 * Line: 108
 * Language: c
 * Block ID: d954578b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 标准中的隐式来源概念
#include <stdlib.h>
#include <stdint.h>

void provenance_basics(void) {
    // 分配获得带来源的指针
    int* p = malloc(sizeof(int) * 10);
    // p 携带来源：指向此 malloc 分配的 40 字节对象

    // 同一来源的派生
    int* q = p + 5;  // q 与 p 同一来源

    // 来源丢失：整数转换
    uintptr_t addr = (uintptr_t)p;  // 来源丢失！
    int* r = (int*)addr;            // 来源恢复（C 实现定义）

    // 严格别名规则
    float* f = (float*)p;  // 新类型，违反严格别名
    // *f = 3.14f;  // 未定义行为！

    free(p);  // 对象生命周期结束
    // p 成为悬挂指针，时间来源失效
}
