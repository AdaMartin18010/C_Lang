/*
 * Auto-generated from: 03_System_Technology_Domains\02_Regex_Engine\02_Pike_VM.md
 * Line: 63
 * Language: c
 * Block ID: 7d007490
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// NFA片段（Fragment）
typedef struct Frag {
    Inst *start;      // 起始指令
    Inst **out;       // 未连接的输出指针列表
} Frag;

// NFA指令
typedef enum {
    CHAR,     // 匹配字符: char c
    SPLIT,    // 分支: split L1, L2
    JMP,      // 跳转: jmp L
    MATCH,    // 匹配成功
    ANY,      // 匹配任意字符
    CLASS,    // 字符类
} InstType;

typedef struct Inst {
    InstType type;
    union {
        char c;           // CHAR
        struct { Inst *x, *y; } sub;  // SPLIT
        Inst *target;     // JMP
        CharClass *cc;    // CLASS
    };
} Inst;

// 基本片段构造
Frag frag_char(char c) {
    Inst *i = malloc(sizeof(Inst));
    i->type = CHAR;
    i->c = c;
    return (Frag){i, &i->out};
}

Frag frag_concat(Frag f1, Frag f2) {
    // f1的输出指向f2的起始
    patch(f1.out, f2.start);
    return (Frag){f1.start, f2.out};
}

Frag frag_alt(Frag f1, Frag f2) {
    Inst *i = malloc(sizeof(Inst));
    i->type = SPLIT;
    i->sub.x = f1.start;
    i->sub.y = f2.start;

    Inst **out = malloc(2 * sizeof(Inst*));
    out[0] = f1.out;
    out[1] = f2.out;
    return (Frag){i, out};
}
