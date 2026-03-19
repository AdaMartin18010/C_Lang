/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\01_Coalgebraic_Theory.md
 * Line: 140
 * Language: c
 * Block ID: 65eaa099
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 流的C语言实现
#include <stdlib.h>
#include <stdint.h>

typedef struct Stream Stream;
struct Stream {
    int head;
    Stream *(*tail)(Stream *);
};

// 无限流：惰性求值
typedef struct {
    int head;
    int (*generator)(int);
    int state;
} LazyStream;

int lazy_head(LazyStream *s) {
    return s->head;
}

Stream *lazy_tail_impl(Stream *s_base) {
    LazyStream *s = (LazyStream *)s_base;
    s->state = s->generator(s->state);
    s->head = s->state;
    return s_base;
}

// 创建自然数流 0, 1, 2, 3, ...
int next_nat(int n) { return n + 1; }

LazyStream *create_nat_stream(void) {
    LazyStream *s = malloc(sizeof(LazyStream));
    s->head = 0;
    s->generator = next_nat;
    s->state = 0;
    return s;
}
