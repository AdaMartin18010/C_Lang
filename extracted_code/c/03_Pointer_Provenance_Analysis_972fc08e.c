/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
 * Line: 328
 * Language: c
 * Block ID: 972fc08e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 显式来源语义示例（概念性）
#ifdef __STDC_VERSION__ >= 202311L
// C23 代码
#include <stdint.h>

void c23_provenance(void) {
    int arr[10];
    int* p = &arr[0];

    // 获取指针的位表示和来源
    uintptr_t addr = (uintptr_t)p;  // 可能保留来源信息

    // 算术操作保留来源
    uintptr_t addr_end = addr + 10 * sizeof(int);  // 来源保持
    int* end = (int*)addr_end;  // one-past-end，来源恢复

    (void)end;

    // 跨对象算术导致来源丢失
    // uintptr_t addr_invalid = addr + 1000;
    // int* invalid = (int*)addr_invalid;  // 未定义行为
}
#endif
