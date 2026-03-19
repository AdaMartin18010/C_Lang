/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Semantic_Equivalence.md
 * Line: 362
 * Language: c
 * Block ID: 18446cb8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 原始代码
int temp = a;
a = b;
b = temp;

// 等价变换（使用XOR技巧，无临时变量）
a = a ^ b;
b = a ^ b;
a = a ^ b;
