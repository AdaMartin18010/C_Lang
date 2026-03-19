/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\04_Compiler_Frontend.md
 * Line: 100
 * Language: c
 * Block ID: d2462184
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Token类型定义
typedef enum {
    TOK_EOF = 0,
    TOK_INT,       // int
    TOK_RETURN,    // return
    TOK_IDENT,     // 标识符
    TOK_NUMBER,    // 数字常量
    TOK_PLUS,      // +
    TOK_MINUS,     // -
    TOK_STAR,      // *
    TOK_SLASH,     // /
    TOK_LPAREN,    // (
    TOK_RPAREN,    // )
    TOK_LBRACE,    // {
    TOK_RBRACE,    // }
    TOK_SEMICOLON, // ;
    TOK_ASSIGN,    // =
} token_type_t;

typedef struct {
    token_type_t type;
    const char*  start;
    int          length;
    int          line;
} token_t;

// 词法分析器状态
typedef struct {
    const char* source;
    const char* current;
    int         line;
} lexer_t;

// 初始化词法分析器
void lexer_init(lexer_t* lex, const char* source) {
    lex->source = source;
    lex->current = source;
    lex->line = 1;
}

// 获取下一个Token
token_t lexer_next_token(lexer_t* lex) {
    skip_whitespace(lex);

    if (is_at_end(lex)) return make_token(lex, TOK_EOF);

    char c = advance(lex);

    // 单字符Token
    switch (c) {
        case '(': return make_token(lex, TOK_LPAREN);
        case ')': return make_token(lex, TOK_RPAREN);
        case '{': return make_token(lex, TOK_LBRACE);
        case '}': return make_token(lex, TOK_RBRACE);
        case ';': return make_token(lex, TOK_SEMICOLON);
        case '+': return make_token(lex, TOK_PLUS);
        case '-': return make_token(lex, TOK_MINUS);
        case '*': return make_token(lex, TOK_STAR);
        case '/': return make_token(lex, TOK_SLASH);
        case '=': return make_token(lex, TOK_ASSIGN);
    }

    // 数字
    if (isdigit(c)) {
        while (isdigit(peek(lex))) advance(lex);
        return make_token(lex, TOK_NUMBER);
    }

    // 标识符/关键字
    if (isalpha(c)) {
        while (isalnum(peek(lex))) advance(lex);
        return make_identifier_token(lex);
    }

    return error_token(lex, "Unexpected character");
}
