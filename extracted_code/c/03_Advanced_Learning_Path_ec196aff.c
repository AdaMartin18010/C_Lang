/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\03_Advanced_Learning_Path.md
 * Line: 841
 * Language: c
 * Block ID: ec196aff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 实现C子集编译器：
// - 数据类型：int, char, 指针, 数组
// - 控制流：if/else, while, for, return
// - 函数：声明、定义、调用
// - 递归下降解析器
// - 简单的IR（三地址码）
// - x86-64代码生成

// 架构：
// 1. 词法分析（手写的lexer）
// 2. 语法分析（递归下降）
// 3. 语义分析（符号表、类型检查）
// 4. IR生成
// 5. 寄存器分配（简单贪心）
// 6. 代码生成
