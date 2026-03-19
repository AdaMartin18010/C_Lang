/*
 * Auto-generated from: 12_Practice_Exercises\12_Mini_Projects.md
 * Line: 141
 * Language: c
 * Block ID: 0acb9224
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef enum {
    TOK_INT,
    TOK_IF,
    TOK_RETURN,
    TOK_IDENTIFIER,
    TOK_NUMBER,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,
    TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE,
    TOK_SEMICOLON,
    TOK_EOF,
} TokenType;

typedef struct {
    TokenType type;
    char *text;
    int line, column;
} Token;

Token* lexer_tokenize(const char *source);
