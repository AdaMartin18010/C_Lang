/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\01_Linear_Logic_Theory.md
 * Line: 536
 * Language: c
 * Block ID: 52212633
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：const不等于线性
const int x = 5;  // x不可变，但可任意复制
void f(const int a) {
    // a可以被使用多次
}

// 线性类型：只能使用一次
// linear int y = ...;
// f(y);  // y被消耗
// g(y);  // 错误：y已被消耗
