/*
 * Auto-generated from: 03_System_Technology_Domains\02_Regex_Engine\README.md
 * Line: 358
 * Language: c
 * Block ID: 88e3eae0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file thompson.c
 * @brief Thompson NFA构造法实现
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_FRAGMENTS 256
#define MAX_STATES 512

typedef struct State State;
struct State {
    int id;
    char c;           /* 转移字符，0表示ε，256表示分裂 */
    State *out;
    State *out1;      /* 用于分裂状态 */
    int last_list;    /* 用于匹配时的标记 */
};

typedef struct {
    State *start;
    State **out;      /* 指向未连接转移的指针列表 */
    int num_out;
} Frag;

typedef struct {
    State states[MAX_STATES];
    int nstate;
} StateList;

State* state(StateList *list, char c, State *out, State *out1) {
    State *s = &list->states[list->nstate++];
    s->id = list->nstate - 1;
    s->c = c;
    s->out = out;
    s->out1 = out1;
    s->last_list = 0;
    return s;
}

/* 创建片段 */
Frag frag(State *start, State **out, int num_out) {
    Frag f = {start, out, num_out};
    return f;
}

/* 连接两个片段 */
Frag concat(Frag f1, Frag f2) {
    /* 将f1的所有未连接指向f2.start */
    for (int i = 0; i < f1.num_out; i++) {
        *f1.out[i] = *f2.start;
    }
    return frag(f1.start, f2.out, f2.num_out);
}

/* 创建选择 */
Frag alternate(StateList *list, Frag f1, Frag f2) {
    State *s = state(list, 256, f1.start, f2.start);  /* 256 = 分裂 */

    /* 合并未连接列表 */
    State **out = malloc((f1.num_out + f2.num_out) * sizeof(State*));
    memcpy(out, f1.out, f1.num_out * sizeof(State*));
    memcpy(out + f1.num_out, f2.out, f2.num_out * sizeof(State*));

    return frag(s, out, f1.num_out + f2.num_out);
}

/* 创建闭包 A* */
Frag star(StateList *list, Frag f) {
    State *s = state(list, 256, f.start, NULL);  /* 分裂状态 */

    /* 将f的所有未连接指向s（循环） */
    for (int i = 0; i < f.num_out; i++) {
        *f.out[i] = *s;
    }

    /* s.out1是新的未连接（匹配0次的情况） */
    State **out = malloc(sizeof(State*));
    out[0] = &s->out1;

    return frag(s, out, 1);
}

/* 解析简单正则表达式并构建NFA (简化版) */
State* parse_regex(StateList *list, const char *pattern);

/* 简化版：只支持基本字符和* */
State* parse_simple(StateList *list, const char *pattern, int *pos) {
    Frag stack[MAX_FRAGMENTS];
    int stack_top = 0;

    while (pattern[*pos] != '\0' && pattern[*pos] != ')') {
        char c = pattern[*pos];

        if (c == '*') {
            /* 将栈顶应用闭包 */
            if (stack_top > 0) {
                stack[stack_top - 1] = star(list, stack[stack_top - 1]);
            }
            (*pos)++;
        } else if (c == '|') {
            /* 处理选择（简化：假设只处理下一个字符） */
            (*pos)++;
            Frag right = parse_simple(list, pattern, pos);
            if (stack_top > 0) {
                stack[stack_top - 1] = alternate(list, stack[stack_top - 1], right);
            }
        } else {
            /* 普通字符 */
            State *s = state(list, c, NULL, NULL);
            State **out = malloc(sizeof(State*));
            out[0] = &s->out;
            stack[stack_top++] = frag(s, out, 1);
            (*pos)++;
        }
    }

    /* 连接所有片段 */
    if (stack_top == 0) {
        /* 空串 */
        State *s = state(list, 0, NULL, NULL);
        State **out = malloc(sizeof(State*));
        out[0] = &s->out;
        return frag(s, out, 1).start;
    }

    Frag result = stack[0];
    for (int i = 1; i < stack_top; i++) {
        result = concat(result, stack[i]);
    }

    /* 添加接受状态 */
    State *accept = state(list, 257, NULL, NULL);  /* 257 = 接受 */
    for (int i = 0; i < result.num_out; i++) {
        *result.out[i] = *accept;
    }

    return result.start;
}

/* 匹配函数 */
#define LIST_SIZE 64

int list_id = 0;

void add_state(State **list, int *n, State *s) {
    if (s == NULL || s->last_list == list_id) return;
    s->last_list = list_id;
    if (s->c == 256) {  /* 分裂状态 */
        add_state(list, n, s->out);
        add_state(list, n, s->out1);
    } else {
        list[(*n)++] = s;
    }
}

bool match(State *start, const char *input) {
    State *current[LIST_SIZE], *next[LIST_SIZE];
    int n_cur = 0, n_next = 0;

    list_id++;
    add_state(current, &n_cur, start);

    for (const char *p = input; ; p++) {
        unsigned char c = *p;

        if (n_cur == 0) return false;

        list_id++;
        n_next = 0;

        for (int i = 0; i < n_cur; i++) {
            State *s = current[i];
            if (s->c == c || (s->c == 257 && c == '\0')) {
                add_state(next, &n_next, s->out);
            }
        }

        /* 检查是否接受 */
        if (c == '\0') {
            for (int i = 0; i < n_cur; i++) {
                if (current[i]->c == 257) return true;
            }
            return false;
        }

        /* 交换current和next */
        memcpy(current, next, n_next * sizeof(State*));
        n_cur = n_next;
    }
}

int main(void) {
    StateList list = {.nstate = 0};

    /* 测试模式 "ab*c" */
    int pos = 0;
    State *start = parse_simple(&list, "ab*c", &pos);

    printf("Pattern: ab*c\n\n");

    const char *tests[] = {"ac", "abc", "abbc", "abbbc", "ab", "a"};
    int n = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < n; i++) {
        printf("Match '%s': %s\n", tests[i], match(start, tests[i]) ? "YES" : "NO");
    }

    return 0;
}
