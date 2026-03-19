/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\12_Compiler_Optimization\README.md
 * Line: 187
 * Language: c
 * Block ID: faafa3b0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 严格别名规则 (C11 6.5§7)
// 不同类型(非char*)的指针不能别名

void func(int *a, long *b) {
    *a = 5;
    *b = 10;
    // 编译器可假设 *a 仍为 5，因为 int* 和 long* 不能别名
    printf("%d\n", *a);  // 可能优化为直接打印 5
}
