/*
 * Auto-generated from: 12_Practice_Exercises\12_Mini_Projects.md
 * Line: 166
 * Language: c
 * Block ID: 469c5eee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 递归下降解析
// 生成AST

typedef struct AstNode AstNode;
struct AstNode {
    enum { AST_FUNC, AST_IF, AST_RETURN, AST_BINARY, AST_CALL, ... } type;
    // ...
    AstNode **children;
};

AstNode* parser_parse(Token *tokens);
