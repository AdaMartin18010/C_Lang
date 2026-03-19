/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 111
 * Language: c
 * Block ID: b50fbcf0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 问题：指针别名导致的数据依赖
void pointer_aliasing_problem(int *a, int *b, int n) {
    for (int i = 1; i < n; i++) {
        #pragma HLS PIPELINE  // 可能无法达到II=1
        a[i] = a[i-1] + b[i];  // HLS工具无法确定a和b是否重叠
    }
}

// 解决方案1：使用restrict关键字
void no_aliasing_restrict(int *restrict a, int *restrict b, int n) {
    for (int i = 1; i < n; i++) {
        #pragma HLS PIPELINE II=1
        a[i] = a[i-1] + b[i];  // 编译器确定无别名，可优化
    }
}

// 解决方案2：使用依赖指令
void explicit_dependency(int *a, int *b, int n) {
    for (int i = 1; i < n; i++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS DEPENDENCE variable=a inter false
        a[i] = a[i-1] + b[i];
    }
}
