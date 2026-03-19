/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\01_Compilation_Build.md
 * Line: 220
 * Language: c
 * Block ID: 6e4866d0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 依赖GCC扩展
void func(void) {
    int arr[__builtin_constant_p(5) ? 5 : 10];  // GCC特有
}

// ✅ 标准C
void func_safe(int n) {
    // C99 VLA（可选）
    int arr[10];  // 固定大小
    // 或动态分配
    int *arr_dyn = malloc(n * sizeof(int));
    free(arr_dyn);
}
