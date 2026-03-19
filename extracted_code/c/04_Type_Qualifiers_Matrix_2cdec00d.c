/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\04_Type_Qualifiers_Matrix.md
 * Line: 116
 * Language: c
 * Block ID: 2cdec00d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 有 restrict: 编译器可假设 p 和 q 不重叠
void copy(int* restrict p, int* restrict q, int n) {
    for (int i = 0; i < n; i++) {
        p[i] = q[i];  // 可优化：预读取q，批量写入p
    }
}

// 无 restrict: 必须考虑 p 和 q 可能重叠（如 copy(a, a+1, n)）
void copy_safe(int* p, int* q, int n) {
    for (int i = 0; i < n; i++) {
        p[i] = q[i];  // 每次循环都重新读取q[i]
    }
}
