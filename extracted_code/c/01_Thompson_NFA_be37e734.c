/*
 * Auto-generated from: 03_System_Technology_Domains\02_Regex_Engine\01_Thompson_NFA.md
 * Line: 46
 * Language: c
 * Block ID: be37e734
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// NFA状态定义
typedef struct State State;
struct State {
    int c;              // 字符或状态类型
    State *out;         // 主要输出
    State *out1;        // 分支输出（用于Split）
    int lastlist;       // 用于匹配时的访问标记
};

// 特殊状态标记
#define Match   256     // 匹配成功状态
#define Split   257     // 分支状态

// 片段（Partial NFA）
typedef struct Frag Frag;
struct Frag {
    State *start;       // 片段起始状态
    State **out;        // 指向未连接输出的指针列表
};

// 基本构造：字符状态
State *state(int c, State *out, State *out1) {
    State *s = malloc(sizeof(State));
    s->c = c;
    s->out = out;
    s->out1 = out1;
    s->lastlist = 0;
    return s;
}

// 构造基本NFA片段
Frag frag_char(int c) {
    State *s = state(c, NULL, NULL);
    Frag f = {s, &s->out};
    return f;
}

// 连接操作: AB
Frag frag_concat(Frag f1, Frag f2) {
    // 将f1的所有未连接输出指向f2.start
    State **p = f1.out;
    while (*p != NULL) {
        *p = f2.start;
        p++;
    }
    Frag f = {f1.start, f2.out};
    return f;
}

// 分支操作: A|B
Frag frag_alt(Frag f1, Frag f2) {
    State *s = state(Split, f1.start, f2.start);

    // 合并输出列表
    int n1 = 0, n2 = 0;
    State **p;
    for (p = f1.out; *p; p++) n1++;
    for (p = f2.out; *p; p++) n2++;

    State **out = malloc((n1 + n2 + 1) * sizeof(State*));
    memcpy(out, f1.out, n1 * sizeof(State*));
    memcpy(out + n1, f2.out, n2 * sizeof(State*));
    out[n1 + n2] = NULL;

    Frag f = {s, out};
    return f;
}

// 闭包: A*
Frag frag_star(Frag f) {
    State *s = state(Split, f.start, NULL);

    // 原片段的结束指向新Split（循环）
    State **p = f.out;
    while (*p != NULL) {
        *p = s;
        p++;
    }

    // 新Split的另一个输出是Match（终止）
    State **out = malloc(2 * sizeof(State*));
    out[0] = &s->out1;
    out[1] = NULL;

    Frag result = {s, out};
    return result;
}
