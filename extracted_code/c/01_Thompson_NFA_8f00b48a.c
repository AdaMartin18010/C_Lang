/*
 * Auto-generated from: 03_System_Technology_Domains\02_Regex_Engine\01_Thompson_NFA.md
 * Line: 138
 * Language: c
 * Block ID: 8f00b48a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单正则解析器（支持基本操作）
// 语法: 连接(隐式) | 分支(|) | 闭包(*) | 分组() | 字符

typedef struct {
    const char *s;  // 输入字符串
    int pos;        // 当前位置
} Parser;

// 前向声明
Frag parse_expr(Parser *p);
Frag parse_term(Parser *p);
Frag parse_factor(Parser *p);

// 解析表达式（分支）
Frag parse_expr(Parser *p) {
    Frag f1 = parse_term(p);

    while (p->s[p->pos] == '|') {
        p->pos++;
        Frag f2 = parse_term(p);
        f1 = frag_alt(f1, f2);
    }

    return f1;
}

// 解析项（连接）
Frag parse_term(Parser *p) {
    Frag f1 = parse_factor(p);

    // 隐式连接：下一个字符是因子开始
    while (p->s[p->pos] && p->s[p->pos] != '|' && p->s[p->pos] != ')') {
        Frag f2 = parse_factor(p);
        f1 = frag_concat(f1, f2);
    }

    return f1;
}

// 解析因子（基本单元或闭包）
Frag parse_factor(Parser *p) {
    Frag f;

    char c = p->s[p->pos];

    if (c == '(') {
        p->pos++;  // 跳过'('
        f = parse_expr(p);
        if (p->s[p->pos] == ')') {
            p->pos++;
        }
    } else if (c == '.') {
        // 通配符
        p->pos++;
        f = frag_char(256);  // 特殊标记
    } else if (isalpha(c) || isdigit(c)) {
        p->pos++;
        f = frag_char(c);
    } else {
        // 错误处理
        f.start = NULL;
        f.out = NULL;
        return f;
    }

    // 检查闭包
    if (p->s[p->pos] == '*') {
        p->pos++;
        f = frag_star(f);
    } else if (p->s[p->pos] == '+') {
        p->pos++;
        // A+ = AA*
        Frag f_star = frag_star(f);
        f = frag_concat(f, f_star);
    } else if (p->s[p->pos] == '?') {
        p->pos++;
        // A? = (A|ε)
        Frag f_empty = {state(Match, NULL, NULL), NULL};
        f = frag_alt(f, f_empty);
    }

    return f;
}

// 编译正则表达式
State *regex_compile(const char *pattern) {
    Parser p = {pattern, 0};
    Frag f = parse_expr(&p);

    // 添加匹配状态
    State *match_state = state(Match, NULL, NULL);
    State **ptr = f.out;
    while (*ptr) {
        *ptr = match_state;
        ptr++;
    }

    return f.start;
}
