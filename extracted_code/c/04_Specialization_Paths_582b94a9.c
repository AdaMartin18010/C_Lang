/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\04_Specialization_Paths.md
 * Line: 542
 * Language: c
 * Block ID: 582b94a9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 实现一个C子集的完整编译器

// 支持特性：
// - 类型：int, char, 指针, 数组
// - 控制流：if/else, while, for, return
// - 函数：声明、定义、调用、递归
// - 优化：常量折叠、死代码消除、寄存器分配

// 架构：
// 1. 词法分析（手写的lexer）
// 2. 语法分析（递归下降）
// 3. 语义分析（符号表、类型检查）
// 4. IR生成（LLVM IR或自定义三地址码）
// 5. 优化pass
// 6. 代码生成（x86-64汇编）
