/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\README.md
 * Line: 472
 * Language: c
 * Block ID: 0a34da8c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 原始代码
int x = 1;
if (cond) x = 2;
return x;

// SSA形式伪代码
int x0 = 1;
if (cond) {
    int x1 = 2;
}
return φ(x0, x1);  // φ函数合并不同路径的值
