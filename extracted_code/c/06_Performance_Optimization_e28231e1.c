/*
 * Auto-generated from: 12_Practice_Exercises\06_Performance_Optimization.md
 * Line: 413
 * Language: c
 * Block ID: e28231e1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

void process(int *data, int n) {
    for (int i = 0; i < n; i++) {
        if (unlikely(data[i] < 0)) {  // 告知编译器这种情况很少
            handle_error();
            continue;
        }
        process_normal(data[i]);
    }
}
