/*
 * Auto-generated from: 03_System_Technology_Domains\02_Regex_Engine\README.md
 * Line: 954
 * Language: c
 * Block ID: 37b2bf31
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file regex_engine.c
 * @brief 简化但完整的正则引擎
 *
 * 支持: . * + ? | () [^a-z] \d \w
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/* ============ 词法分析器 ============ */

typedef enum {
    TOK_CHAR, TOK_DOT, TOK_STAR, TOK_PLUS, TOK_QUESTION,
    TOK_PIPE, TOK_LPAREN, TOK_RPAREN, TOK_LBRACKET, TOK_RBRACKET,
    TOK_CHARSET, TOK_EOF
} TokenType;

typedef struct {
    TokenType type;
    char c;              /* 对于TOK_CHAR */
    char *charset;       /* 对于TOK_CHARSET */
} Token;

typedef struct {
    const char *input;
    int pos;
} Lexer;

void lexer_init(Lexer *lex, const char *input) {
    lex->input = input;
    lex->pos = 0;
}

/* 解析字符集 [abc] 或 [^abc] */
char* parse_charset(Lexer *lex) {
    bool negated = false;
    if (lex->input[lex->pos] == '^') {
        negated = true;
        lex->pos++;
    }

    char *set = malloc(256);
    memset(set, negated ? 1 : 0, 256);

    while (lex->input[lex->pos] != ']' && lex->input[lex->pos] != '\0') {
        char c = lex->input[lex->pos++];
        set[(unsigned char)c] = negated ? 0 : 1;
    }

    if (lex->input[lex->pos] == ']') lex->pos++;
    return set;
}

Token lexer_next(Lexer *lex) {
    Token tok = {TOK_EOF, 0, NULL};
    char c = lex->input[lex->pos];

    if (c == '\0') return tok;
    lex->pos++;

    switch (c) {
        case '.': tok.type = TOK_DOT; break;
        case '*': tok.type = TOK_STAR; break;
        case '+': tok.type = TOK_PLUS; break;
        case '?': tok.type = TOK_QUESTION; break;
        case '|': tok.type = TOK_PIPE; break;
        case '(': tok.type = TOK_LPAREN; break;
        case ')': tok.type = TOK_RPAREN; break;
        case '[':
            tok.type = TOK_CHARSET;
            tok.charset = parse_charset(lex);
            break;
        case '\\':
            c = lex->input[lex->pos++];
            switch (c) {
                case 'd':
                    tok.type = TOK_CHARSET;
                    tok.charset = malloc(256);
                    memset(tok.charset, 0, 256);
                    for (int i = '0'; i <= '9'; i++) tok.charset[i] = 1;
                    break;
                case 'w':
                    tok.type = TOK_CHARSET;
                    tok.charset = malloc(256);
                    memset(tok.charset, 0, 256);
                    for (int i = 'a'; i <= 'z'; i++) tok.charset[i] = 1;
                    for (int i = 'A'; i <= 'Z'; i++) tok.charset[i] = 1;
                    for (int i = '0'; i <= '9'; i++) tok.charset[i] = 1;
                    tok.charset['_'] = 1;
                    break;
                default:
                    tok.type = TOK_CHAR;
                    tok.c = c;
            }
            break;
        default:
            tok.type = TOK_CHAR;
            tok.c = c;
    }

    return tok;
}

/* ============ 语法分析 & NFA构建 ============ */

typedef struct Inst Inst;
struct Inst {
    enum { I_CHAR, I_CHARSET, I_SPLIT, I_JMP, I_MATCH, I_SAVE } op;
    char c;
    char *charset;
    Inst *x, *y;
    int save_id;
};

typedef struct {
    Inst *start;
    Inst **out;
    int nout;
} Frag;

typedef struct {
    Inst *insts;
    int ninst;
    int maxinst;
    Token current;
    Lexer *lexer;
} Parser;

void parser_init(Parser *p, Lexer *lex) {
    p->insts = malloc(256 * sizeof(Inst));
    p->ninst = 0;
    p->maxinst = 256;
    p->lexer = lex;
    p->current = lexer_next(lex);
}

Inst* emit(Parser *p, Inst i) {
    if (p->ninst >= p->maxinst) {
        p->maxinst *= 2;
        p->insts = realloc(p->insts, p->maxinst * sizeof(Inst));
    }
    p->insts[p->ninst] = i;
    return &p->insts[p->ninst++];
}

void advance(Parser *p) {
    p->current = lexer_next(p->lexer);
}

Frag make_frag(Inst *start, Inst **out, int nout) {
    Frag f = {start, out, nout};
    return f;
}

Frag concat(Frag f1, Frag f2) {
    for (int i = 0; i < f1.nout; i++) {
        *f1.out[i] = *f2.start;
    }
    free(f1.out);
    return make_frag(f1.start, f2.out, f2.nout);
}

Frag alternate(Parser *p, Frag f1, Frag f2) {
    Inst *split = emit(p, (Inst){I_SPLIT, 0, NULL, f1.start, f2.start, 0});
    Inst **out = malloc((f1.nout + f2.nout) * sizeof(Inst*));
    memcpy(out, f1.out, f1.nout * sizeof(Inst*));
    memcpy(out + f1.nout, f2.out, f2.nout * sizeof(Inst*));
    free(f1.out);
    free(f2.out);
    return make_frag(split, out, f1.nout + f2.nout);
}

Frag star(Parser *p, Frag f) {
    Inst *split = emit(p, (Inst){I_SPLIT, 0, NULL, NULL, NULL, 0});
    Inst *jump = emit(p, (Inst){I_JMP, 0, NULL, split, NULL, 0});

    for (int i = 0; i < f.nout; i++) {
        *f.out[i] = *split;
    }
    free(f.out);

    split->x = f.start;
    split->y = jump;

    Inst **out = malloc(sizeof(Inst*));
    out[0] = jump;
    return make_frag(split, out, 1);
}

Frag plus(Parser *p, Frag f) {
    Frag fstar = star(p, f);
    return concat(f, fstar);
}

Frag question(Parser *p, Frag f) {
    Inst *split = emit(p, (Inst){I_SPLIT, 0, NULL, f.start, NULL, 0});
    Inst **out = malloc((f.nout + 1) * sizeof(Inst*));
    memcpy(out, f.out, f.nout * sizeof(Inst*));
    out[f.nout] = &split->y;
    free(f.out);
    return make_frag(split, out, f.nout + 1);
}

Frag parse_atom(Parser *p);
Frag parse_expr(Parser *p);

Frag parse_factor(Parser *p) {
    Frag f = parse_atom(p);

    while (p->current.type == TOK_STAR ||
           p->current.type == TOK_PLUS ||
           p->current.type == TOK_QUESTION) {
        TokenType t = p->current.type;
        advance(p);

        if (t == TOK_STAR) f = star(p, f);
        else if (t == TOK_PLUS) f = plus(p, f);
        else f = question(p, f);
    }

    return f;
}

Frag parse_sequence(Parser *p) {
    Frag f = parse_factor(p);

    while (p->current.type != TOK_EOF &&
           p->current.type != TOK_PIPE &&
           p->current.type != TOK_RPAREN) {
        Frag f2 = parse_factor(p);
        f = concat(f, f2);
    }

    return f;
}

Frag parse_expr(Parser *p) {
    Frag f = parse_sequence(p);

    while (p->current.type == TOK_PIPE) {
        advance(p);
        Frag f2 = parse_sequence(p);
        f = alternate(p, f, f2);
    }

    return f;
}

Frag parse_atom(Parser *p) {
    Frag f;
    Inst *i;

    switch (p->current.type) {
        case TOK_CHAR:
            i = emit(p, (Inst){I_CHAR, p->current.c, NULL, NULL, NULL, 0});
            advance(p);
            f = make_frag(i, malloc(sizeof(Inst*)), 1);
            f.out[0] = i;
            break;

        case TOK_DOT:
            i = emit(p, (Inst){I_CHARSET, 0, NULL, NULL, NULL, 0});
            i->charset = malloc(256);
            memset(i->charset, 1, 256);
            i->charset['\n'] = 0;
            advance(p);
            f = make_frag(i, malloc(sizeof(Inst*)), 1);
            f.out[0] = i;
            break;

        case TOK_CHARSET:
            i = emit(p, (Inst){I_CHARSET, 0, p->current.charset, NULL, NULL, 0});
            advance(p);
            f = make_frag(i, malloc(sizeof(Inst*)), 1);
            f.out[0] = i;
            break;

        case TOK_LPAREN:
            advance(p);
            f = parse_expr(p);
            if (p->current.type == TOK_RPAREN) advance(p);
            break;

        default:
            /* 空串 */
            i = emit(p, (Inst){I_JMP, 0, NULL, NULL, NULL, 0});
            f = make_frag(i, malloc(sizeof(Inst*)), 1);
            f.out[0] = i;
            break;
    }

    return f;
}

/* ============ 匹配引擎 ============ */

#define LIST_SIZE 256

int list_id = 0;

typedef struct {
    Inst *pc;
    const char *sp;
} Thread;

void add_thread(Thread *list, int *n, Inst *pc, const char *sp) {
    if (pc->op == I_SPLIT) {
        add_thread(list, n, pc->x, sp);
        add_thread(list, n, pc->y, sp);
    } else if (pc->op == I_JMP) {
        add_thread(list, n, pc->x, sp);
    } else {
        list[*n].pc = pc;
        list[*n].sp = sp;
        (*n)++;
    }
}

bool match_regex(Inst *prog, const char *text) {
    Thread current[LIST_SIZE], next[LIST_SIZE];
    int n_cur = 0, n_next = 0;

    add_thread(current, &n_cur, prog, text);

    for (const char *sp = text; ; sp++) {
        n_next = 0;

        for (int i = 0; i < n_cur; i++) {
            Thread *t = &current[i];

            switch (t->pc->op) {
                case I_CHAR:
                    if (*sp == t->pc->c) {
                        add_thread(next, &n_next, t->pc + 1, sp + 1);
                    }
                    break;

                case I_CHARSET:
                    if (*sp != '\0' && t->pc->charset[(unsigned char)*sp]) {
                        add_thread(next, &n_next, t->pc + 1, sp + 1);
                    }
                    break;

                case I_MATCH:
                    return true;

                default:
                    break;
            }
        }

        if (*sp == '\0') break;

        memcpy(current, next, n_next * sizeof(Thread));
        n_cur = n_next;
    }

    /* 检查接受状态 */
    for (int i = 0; i < n_cur; i++) {
        if (current[i].pc->op == I_MATCH) return true;
    }

    return false;
}

/* ============ 主程序 ============ */

bool regex_match(const char *pattern, const char *text) {
    Lexer lex;
    lexer_init(&lex, pattern);

    Parser parser;
    parser_init(&parser, &lex);

    Frag f = parse_expr(&parser);

    /* 添加匹配指令 */
    Inst *match = emit(&parser, (Inst){I_MATCH, 0, NULL, NULL, NULL, 0});
    for (int i = 0; i < f.nout; i++) {
        *f.out[i] = *match;
    }

    return match_regex(f.start, text);
}

int main(void) {
    printf("=== Regex Engine Demo ===\n\n");

    struct { const char *pat, *text; } tests[] = {
        {"abc", "abc"},
        {"a*b", "aaab"},
        {"a*b", "b"},
        {"a|b", "a"},
        {"a|b", "c"},
        {"colou?r", "color"},
        {"colou?r", "colour"},
        {"a+", "aaa"},
        {"a+", ""},
        {"\\d+", "12345"},
        {"\\w+", "hello_world"},
        {"a.*b", "axxxb"},
        {"[aeiou]+", "aei"},
        {"[^aeiou]+", "bcd"},
        {NULL, NULL}
    };

    for (int i = 0; tests[i].pat != NULL; i++) {
        bool result = regex_match(tests[i].pat, tests[i].text);
        printf("Pattern '%s' matches '%s': %s\n",
               tests[i].pat, tests[i].text,
               result ? "YES" : "NO");
    }

    return 0;
}
