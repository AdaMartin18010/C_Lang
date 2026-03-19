/*
 * Auto-generated from: 03_System_Technology_Domains\16_Rust_Interoperability\01_C_ABI_Basics.md
 * Line: 288
 * Language: c
 * Block ID: 43b2c773
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// === rust_client.c ===
#include <stdio.h>
#include <stdlib.h>

// Rust暴露的函数
extern char* rust_process_data(const char* input);
extern void rust_string_free(char* s);

extern void* processor_create(void);
extern int processor_feed(void* handle, const uint8_t* data, size_t len);
extern void processor_destroy(void* handle);

int main() {
    // 测试字符串处理
    const char* input = "hello, rust!";
    char* result = rust_process_data(input);
    printf("Result: %s\n", result);
    rust_string_free(result);  // 必须释放！

    // 测试处理器
    void* proc = processor_create();
    uint8_t data[] = {1, 2, 3, 4, 5};
    processor_feed(proc, data, sizeof(data));
    processor_destroy(proc);

    return 0;
}
