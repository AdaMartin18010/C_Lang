/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Depth.md
 * Line: 405
 * Language: c
 * Block ID: a9b01133
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 无restrict - 编译器必须假设别名 */
void add(int *a, int *b, int n) {
    for (int i = 0; i < n; i++)
        a[i] += b[i];  /* 每次循环都要加载a[i]和b[i] */
}

/* 有restrict - 允许更多优化 */
void add_restrict(int *restrict a, int *restrict b, int n) {
    for (int i = 0; i < n; i++)
        a[i] += b[i];  /* 编译器可以缓存值 */
}
