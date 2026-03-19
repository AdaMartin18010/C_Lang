/*
 * Auto-generated from: 03_System_Technology_Domains\02_Regex_Engine\README.md
 * Line: 580
 * Language: c
 * Block ID: fe042e46
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file backtrack.c
 * @brief NFA回溯匹配算法
 *
 * 支持捕获组和反向引用
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_GROUPS 10
#define MAX_STATES 256

typedef struct Inst Inst;
struct Inst {
    enum { CHAR, SPLIT, JMP, MATCH, SAVE } opcode;
    char c;              /* 对于CHAR */
    Inst *x, *y;         /* 对于SPLIT/JMP */
    int n;               /* 对于SAVE: 保存的组号 */
};

typedef struct {
    const char *sp;      /* 字符串位置 */
    Inst *pc;            /* 程序计数器 */
} Thread;

/* 简单指令创建 */
Inst* inst_char(char c) {
    Inst *i = malloc(sizeof(Inst));
    i->opcode = CHAR;
    i->c = c;
    return i;
}

Inst* inst_split(Inst *x, Inst *y) {
    Inst *i = malloc(sizeof(Inst));
    i->opcode = SPLIT;
    i->x = x;
    i->y = y;
    return i;
}

Inst* inst_jmp(Inst *x) {
    Inst *i = malloc(sizeof(Inst));
    i->opcode = JMP;
    i->x = x;
    return i;
}

Inst* inst_save(int n) {
    Inst *i = malloc(sizeof(Inst));
    i->opcode = SAVE;
    i->n = n;
    return i;
}

Inst* inst_match(void) {
    Inst *i = malloc(sizeof(Inst));
    i->opcode = MATCH;
    return i;
}

/* 匹配上下文 */
typedef struct {
    const char *input;
    const char *groups[MAX_GROUPS * 2];  /* 起始/结束位置 */
    int matched;
} MatchContext;

/* 递归回溯匹配 */
bool backtrack(MatchContext *ctx, Inst *pc, const char *sp) {
    for (;;) {
        switch (pc->opcode) {
            case CHAR:
                if (*sp != pc->c) return false;
                pc++;
                sp++;
                break;

            case SPLIT:
                /* 尝试两个分支 */
                if (backtrack(ctx, pc->x, sp)) return true;
                pc = pc->y;
                break;

            case JMP:
                pc = pc->x;
                break;

            case SAVE:
                {
                    const char *old = ctx->groups[pc->n];
                    ctx->groups[pc->n] = sp;
                    if (backtrack(ctx, pc + 1, sp)) return true;
                    ctx->groups[pc->n] = old;  /* 回溯 */
                    return false;
                }

            case MATCH:
                ctx->matched = 1;
                return true;
        }
    }
}

/* 构建简单程序: (a|b)*c */
Inst* build_program(void) {
    /* 简化：直接构建 */
    Inst *match = inst_match();

    Inst *c = inst_char('c');
    c->x = match;

    Inst *a = inst_char('a');
    Inst *b = inst_char('b');

    Inst *split = inst_split(a, b);
    a->x = split;
    b->x = split;

    Inst *start = inst_split(c, split);
    split->x = c;

    return start;
}

int main(void) {
    Inst *prog = build_program();

    printf("Pattern: (a|b)*c\n\n");

    const char *tests[] = {"c", "ac", "bc", "abc", "abac", "d"};
    int n = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < n; i++) {
        MatchContext ctx = {0};
        ctx.input = tests[i];
        bool result = backtrack(&ctx, prog, tests[i]);
        printf("Match '%s': %s\n", tests[i], result ? "YES" : "NO");
    }

    return 0;
}
