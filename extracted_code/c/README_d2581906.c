/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\README.md
 * Line: 198
 * Language: c
 * Block ID: d2581906
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 的 cleanup 属性扩展
#ifdef __GNUC__
#define AUTO_FREE __attribute__((cleanup(auto_free_func)))

static inline void auto_free_func(void* p) {
    free(*(void**)p);
}
#else
#define AUTO_FREE
#endif

void example(void) {
    AUTO_FREE char* buffer = malloc(1024);
    // 使用 buffer...
    // 函数退出时自动释放
}
