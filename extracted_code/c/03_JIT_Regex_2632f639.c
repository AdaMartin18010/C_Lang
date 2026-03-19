/*
 * Auto-generated from: 03_System_Technology_Domains\03_JIT_Regex.md
 * Line: 358
 * Language: c
 * Block ID: 2632f639
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 性能测试框架
#include <time.h>

double benchmark_regex(void *regex, const char *input, int iterations) {
    clock_t start = clock();

    for (int i = 0; i < iterations; i++) {
        regex_match(regex, input, strlen(input));
    }

    clock_t end = clock();
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}
