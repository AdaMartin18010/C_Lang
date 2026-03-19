/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\01_Mental_Models.md
 * Line: 246
 * Language: c
 * Block ID: 7e5b0344
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 代码结构反映问题结构

// 编译器的阶段对应心智模型：
// 源代码 -> 词法分析 -> 语法分析 -> 语义分析 -> 代码生成 -> 目标代码

// 词法分析器
TokenStream *lexical_analysis(const char *source);

// 语法分析器
AST *syntax_analysis(TokenStream *tokens);

// 语义分析器
SemanticInfo *semantic_analysis(AST *ast);

// 代码生成
ObjectCode *code_generation(SemanticInfo *info);

// 这种结构帮助读者建立编译器的心智模型
