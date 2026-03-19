/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Semantic_Equivalence.md
 * Line: 338
 * Language: c
 * Block ID: 86238062
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例1: 有符号整数溢出
int x = INT_MAX;
x = x + 1;  // UB! 可假设永不发生

// 示例2: 空指针解引用
int *p = NULL;
*p = 42;    // UB! 编译器可优化整个分支

// 示例3: 严格别名违规
float f = 1.0f;
int i = *(int*)&f;  // UB! 违反严格别名规则
