/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\README.md
 * Line: 323
 * Language: c
 * Block ID: c872e786
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 无限序列的惰性表示
typedef struct IntStream {
    int value;
    struct IntStream* (*next)(int prev);
} IntStream;

// 自然数流
IntStream* nat_stream(int n) {
    IntStream* s = malloc(sizeof(IntStream));
    s->value = n;
    s->next = nat_stream;
    return s;
}

// 筛选操作 (共归纳定义)
IntStream* filter(IntStream* s, bool (*pred)(int)) {
    if (s == NULL) return NULL;

    IntStream* filtered = malloc(sizeof(IntStream));

    if (pred(s->value)) {
        filtered->value = s->value;
        filtered->next = lambda(int x) {
            return filter(s->next(s->value), pred);
        };
    } else {
        // 递归但不构造 (尾调用优化)
        free(filtered);
        return filter(s->next(s->value), pred);
    }

    return filtered;
}
