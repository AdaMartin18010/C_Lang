/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\03_Pointer_Concepts_Map.md
 * Line: 92
 * Language: c
 * Block ID: 360f1c65
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 语法解析（从右向左读）
int (*fp)(int, int)
// fp是指针，指向接受两个int、返回int的函数

// typedef简化
typedef int (*BinaryOp)(int, int);
BinaryOp ops[] = {add, sub, mul, div};
result = ops[0](a, b);  // 调用add
