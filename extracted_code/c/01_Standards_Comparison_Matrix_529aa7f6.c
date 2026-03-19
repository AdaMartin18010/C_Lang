/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 2535
 * Language: c
 * Block ID: 529aa7f6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 可能的边界检查数组 */
[[bounds_check]]
void safe_function(int array[static 10]) {
    /* 编译器和运行时检查数组边界 */
    array[10] = 0;  /* 编译错误或运行时检查 */
}

/* 空指针检查 */
[[nonnull]]
void process_string([[nonnull]] const char* str) {
    /* 编译器假设str不为null */
    printf("%s\n", str);
}
