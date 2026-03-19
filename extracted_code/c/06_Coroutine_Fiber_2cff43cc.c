/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\06_Coroutine_Fiber.md
 * Line: 556
 * Language: c
 * Block ID: 2cff43cc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 无限序列生成器 */

typedef struct {
    int (*next)(void*);
    void* state;
} generator_t;

/* 范围生成器 */
int range_next(void* state) {
    int* st = state;
    return (*st)++;
}

generator_t range(int start) {
    int* state = malloc(sizeof(int));
    *state = start;
    return (generator_t){range_next, state};
}

/* 过滤生成器 */
typedef struct {
    generator_t* source;
    int (*pred)(int);
} filter_state_t;

int filter_next(void* state) {
    filter_state_t* fs = state;
    while (1) {
        int val = fs->source->next(fs->source->state);
        if (fs->pred(val)) return val;
    }
}

generator_t filter(generator_t* src, int (*p)(int)) {
    filter_state_t* st = malloc(sizeof(filter_state_t));
    st->source = src;
    st->pred = p;
    return (generator_t){filter_next, st};
}

/* 使用 */
void example_generator(void) {
    generator_t nums = range(1);
    generator_t evens = filter(&nums, is_even);

    for (int i = 0; i < 10; i++) {
        printf("%d ", evens.next(evens.state));
    }
}
