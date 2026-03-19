/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\06_Coroutine_Fiber.md
 * Line: 339
 * Language: c
 * Block ID: cfe0e89d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 使用switch实现的协程 (Duff's device) */

#define cr_begin static int __state = 0; switch(__state) { case 0:
#define cr_yield(x) do { __state = __LINE__; return x; case __LINE__:; } while(0)
#define cr_end } __state = 0
#define cr_reset __state = 0

typedef struct {
    int value;
    int __state;
} generator_t;

/* 生成器示例 */
int fibonacci(generator_t* g) {
    switch(g->__state) {
        case 0:
            g->value = 0;
            g->__state = 1;
            return g->value;

        case 1:
            g->value = 1;
            g->__state = 2;
            return g->value;

        case 2:
            while (1) {
                int next = g->value +
                    ((int*)g)[-1];  /* 需要保存前一个值 */
                ((int*)g)[-1] = g->value;
                g->value = next;
                g->__state = 3;
                return g->value;

        case 3:;
            }
    }
    return -1;
}

/* 使用 */
void use_generator(void) {
    generator_t gen = {0};

    for (int i = 0; i < 10; i++) {
        printf("%d ", fibonacci(&gen));
    }
    printf("\n");
}
