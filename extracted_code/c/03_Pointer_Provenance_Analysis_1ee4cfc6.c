/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
 * Line: 833
 * Language: c
 * Block ID: 1ee4cfc6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 指针算术与来源
void pointer_arithmetic_provenance(void) {
    int arr[10];
    int* base = &arr[0];

    // 来源保持的操作
    int* p1 = base + 5;      // OK: 在数组内
    int* p2 = base + 10;     // OK: one-past-end（来源保持，但不能解引用）
    ptrdiff_t diff = p2 - p1; // OK: 5

    // 来源丢失的操作（未定义行为）
    // int* p3 = base + 11;  // 越界，来源丢失
    // int* p4 = base - 1;   // 低于起始，来源丢失
    // int x = *p2;          // 解引用 one-past-end，未定义行为

    (void)diff;
}

// 指针比较规则
void pointer_comparison(void) {
    int arr1[10];
    int arr2[10];

    int* p1 = &arr1[5];
    int* p2 = &arr1[8];
    int* p3 = &arr2[0];

    // 同一来源的比较
    _Bool same_array = p1 < p2;  // OK: 可以比较

    // 不同来源的比较（未定义行为，除非都是 one-past-end）
    // _Bool diff_array = p1 < p3;  // 未定义行为！

    (void)same_array;
}
