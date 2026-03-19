/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\04_Compiler_Frontend.md
 * Line: 181
 * Language: c
 * Block ID: c9eb4bfe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// AST节点类型
typedef enum {
    AST_PROGRAM,
    AST_FUNCTION,
    AST_RETURN,
    AST_BINARY,
    AST_NUMBER,
    AST_IDENTIFIER
} ast_node_type_t;

typedef struct ast_node {
    ast_node_type_t type;
    union {
        // 函数定义
        struct { char* name; struct ast_node* body; } func;
        // 返回语句
        struct { struct ast_node* value; } ret;
        // 二元运算
        struct { token_type_t op;
                 struct ast_node* left;
                 struct ast_node* right; } binary;
        // 数字常量
        struct { int value; } number;
    } as;
} ast_node_t;

typedef struct {
    lexer_t* lexer;
    token_t  current;
    token_t  previous;
    bool     had_error;
} parser_t;

// 语法分析入口
ast_node_t* parse(parser_t* parser) {
    return parse_program(parser);
}

// program ::= function
ast_node_t* parse_program(parser_t* parser) {
    return parse_function(parser);
}

// function ::= "int" identifier "(" ")" "{" statement "}"
ast_node_t* parse_function(parser_t* parser) {
    consume(parser, TOK_INT, "Expect 'int' at start of function");
    consume(parser, TOK_IDENT, "Expect function name");
    char* name = copy_string(parser->previous);

    consume(parser, TOK_LPAREN, "Expect '(' after function name");
    consume(parser, TOK_RPAREN, "Expect ')' after parameters");
    consume(parser, TOK_LBRACE, "Expect '{' before function body");

    ast_node_t* body = parse_statement(parser);

    consume(parser, TOK_RBRACE, "Expect '}' after function body");

    return make_function_node(name, body);
}

// statement ::= "return" expression ";"
ast_node_t* parse_statement(parser_t* parser) {
    consume(parser, TOK_RETURN, "Expect 'return' statement");
    ast_node_t* value = parse_expression(parser);
    consume(parser, TOK_SEMICOLON, "Expect ';' after return value");
    return make_return_node(value);
}

// expression ::= term (("+" | "-") term)*
ast_node_t* parse_expression(parser_t* parser) {
    ast_node_t* node = parse_term(parser);

    while (match(parser, TOK_PLUS) || match(parser, TOK_MINUS)) {
        token_type_t op = parser->previous.type;
        ast_node_t* right = parse_term(parser);
        node = make_binary_node(op, node, right);
    }

    return node;
}

// term ::= factor (("*" | "/") factor)*
ast_node_t* parse_term(parser_t* parser) {
    ast_node_t* node = parse_factor(parser);

    while (match(parser, TOK_STAR) || match(parser, TOK_SLASH)) {
        token_type_t op = parser->previous.type;
        ast_node_t* right = parse_factor(parser);
        node = make_binary_node(op, node, right);
    }

    return node;
}

// factor ::= number | "(" expression ")"
ast_node_t* parse_factor(parser_t* parser) {
    if (match(parser, TOK_NUMBER)) {
        int value = strtol(parser->previous.start, NULL, 10);
        return make_number_node(value);
    }

    if (match(parser, TOK_LPAREN)) {
        ast_node_t* expr = parse_expression(parser);
        consume(parser, TOK_RPAREN, "Expect ')' after expression");
        return expr;
    }

    error(parser, "Expect expression");
    return NULL;
}
