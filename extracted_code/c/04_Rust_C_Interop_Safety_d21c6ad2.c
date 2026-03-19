/*
 * Auto-generated from: 08_Zig_C_Connection\04_Rust_C_Interop_Safety.md
 * Line: 2738
 * Language: c
 * Block ID: d21c6ad2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* callback_user.c - 使用Rust回调 */
#include <stdio.h>
#include <stdlib.h>

/* Rust导出的回调函数 */
extern int rust_is_even(int value, void* user_data);
extern double rust_square(double value, void* user_data);
extern int rust_max_finder(const void* a, const void* b);

/* 回调表结构 */
typedef struct {
    int (*predicate)(int, void*);
    double (*transform)(double, void*);
    int (*compare)(const void*, const void*);
} CallbackTable;

/* Rust函数 */
extern void rust_register_callbacks(CallbackTable* table);
extern int rust_process_with_callbacks(
    const double* data,
    size_t len,
    const CallbackTable* table,
    double* results
);

/* 使用Rust回调的C函数 */
void test_rust_callbacks(void) {
    printf("=== 测试Rust回调 ===\n");

    /* 注册回调 */
    CallbackTable table = {0};
    rust_register_callbacks(&table);

    /* 使用predicate回调 */
    printf("测试 rust_is_even:\n");
    for (int i = 0; i < 5; i++) {
        int result = rust_is_even(i, NULL);
        printf("  rust_is_even(%d) = %d\n", i, result);
    }

    /* 使用transform回调 */
    printf("\n测试 rust_square:\n");
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double results[5];

    rust_process_with_callbacks(data, 5, &table, results);
    printf("  原始: ");
    for (int i = 0; i < 5; i++) printf("%.1f ", data[i]);
    printf("\n  平方: ");
    for (int i = 0; i < 5; i++) printf("%.1f ", results[i]);
    printf("\n");

    /* 使用compare回调排序 */
    printf("\n测试 rust_max_finder (排序):\n");
    double unsorted[] = {3.5, 1.2, 4.8, 2.1, 5.0};
    printf("  排序前: ");
    for (int i = 0; i < 5; i++) printf("%.1f ", unsorted[i]);

    qsort(unsorted, 5, sizeof(double), rust_max_finder);

    printf("\n  排序后: ");
    for (int i = 0; i < 5; i++) printf("%.1f ", unsorted[i]);
    printf("\n");
}

int main(void) {
    test_rust_callbacks();
    return 0;
}
