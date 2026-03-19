/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\12_Compiler_Optimization\README.md
 * Line: 363
 * Language: c
 * Block ID: cc4f536d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 推荐：利于向量化
void vector_friendly(int * restrict a, int * restrict b, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = b[i] * 2;
    }
}

// 避免：难以向量化
void not_vector_friendly(int *a, int *b, int n) {
    for (int i = 0; i < n; i++) {
        if (i % 2 == 0)  // 不规则控制流
            a[i] = b[i] * 2;
        else
            a[i] = b[i] * 3;
    }
}
