/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
 * Line: 907
 * Language: c
 * Block ID: d29e43e5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 越界指针行为
void out_of_bounds_provenance(void) {
    int arr[10];
    int* base = arr;

    // 编译时可以检测的一些情况
    // int* p = base + 20;  // 某些编译器警告

    // 运行时越界
    int index = 100;
    int* p = base + index;  // 越界，来源丢失

    // 即使不使用 p 解引用，某些操作也是 UB
    // 例如，将 p 与其他指针比较

    (void)p;
}

// 动态分配的情况
void heap_out_of_bounds(void) {
    int* p = malloc(10 * sizeof(int));
    if (!p) return;

    // 合法范围: [p, p+10)
    int* end = p + 10;  // OK: one-past-end

    // int* bad = p + 20;  // 越界，来源丢失

    free(p);
    // p 现在没有有效来源（时间来源失效）

    (void)end;
}
