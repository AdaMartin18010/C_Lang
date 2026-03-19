/*
 * Auto-generated from: 06_Thinking_Representation\07_Knowledge_Graph\04_Type_System_Knowledge_Graph.md
 * Line: 277
 * Language: c
 * Block ID: 7a1d5ad5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

char c = 'A';
int i = 100;
float f = 3.14f;

// c 先提升为 int，然后与 i 相加，结果提升为 float
float result = c + i + f;   // char → int → float

// 不同类型运算
long l = 1000L;
unsigned int ui = 500U;
// 如果 sizeof(long) == sizeof(int)，两者都转为 unsigned long
auto r = l + ui;  // 结果类型取决于平台
