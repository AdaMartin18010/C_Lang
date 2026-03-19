/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\02_Type_System_Matrix.md
 * Line: 336
 * Language: c
 * Block ID: 952d5ee8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 整型提升顺序
char → int
short → int
// 如果int能表示原类型所有值，则提升为int
// 否则提升为unsigned int

void example() {
    char c = 'A';
    // c 在使用时提升为 int
    int x = c + 1;  // c 先提升为 int

    // 混合类型运算
    short s = 10;
    long l = s + 1;  // s → int → long
}
