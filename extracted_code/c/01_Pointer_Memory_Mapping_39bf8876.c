/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\01_Pointer_Memory_Mapping.md
 * Line: 383
 * Language: c
 * Block ID: 39bf8876
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化前 - 编译器假设指针可能别名
void add(int *a, int *b, int *c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];  // 编译器需考虑a/b/c重叠
    }
}

// 优化后 - 使用restrict消除别名
void add_restrict(int *restrict a, int *restrict b,
                  int *restrict c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];  // 编译器可优化向量化
    }
}
