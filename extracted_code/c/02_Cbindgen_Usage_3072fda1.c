/*
 * Auto-generated from: 03_System_Technology_Domains\02_Cbindgen_Usage.md
 * Line: 1352
 * Language: c
 * Block ID: 3072fda1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// main.c - C代码调用Rust生成的库

#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"

int main() {
    // 初始化库
    mylib_init();

    // 使用计数器
    ThreadSafeCounter* counter = mylib_counter_create();
    if (!counter) {
        fprintf(stderr, "Failed to create counter\n");
        return 1;
    }

    mylib_counter_increment(counter);
    mylib_counter_increment(counter);
    uint64_t value = mylib_counter_get(counter);
    printf("Counter value: %llu\n", (unsigned long long)value);

    mylib_counter_destroy(counter);

    // 使用字符串处理
    char* greeting = mylib_greet("World");
    if (greeting) {
        printf("%s\n", greeting);
        mylib_string_free(greeting);
    }

    // 错误处理示例
    int result = mylib_process("valid utf-8 string");
    if (result != 0) {
        int code = mylib_last_error_code();
        char* msg = mylib_last_error_message();
        fprintf(stderr, "Error %d: %s\n", code, msg);
        mylib_string_free(msg);
        mylib_clear_error();
    }

    // 清理
    mylib_cleanup();

    return 0;
}
