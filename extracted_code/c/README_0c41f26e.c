/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\README.md
 * Line: 581
 * Language: c
 * Block ID: 0c41f26e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 未使用变量
int __attribute__((unused)) debug_flag = 0;

// 清理函数
void free_buffer(void* p) {
    free(*(void**)p);
}

void process_data(void) {
    char* buffer __attribute__((cleanup(free_buffer))) = malloc(1024);
    // buffer 在作用域结束时自动释放
    // ...
}

// 弱符号（可被覆盖）
int __attribute__((weak)) weak_variable = 0;
void __attribute__((weak)) weak_function(void) {}

// 别名
void original_function(void) { /* ... */ }
void __attribute__((alias("original_function"))) aliased_function(void);

// 可见性
void __attribute__((visibility("default"))) public_api(void);
void __attribute__((visibility("hidden"))) internal_helper(void);
