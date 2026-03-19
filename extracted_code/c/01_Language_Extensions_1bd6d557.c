/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\01_Language_Extensions.md
 * Line: 54
 * Language: c
 * Block ID: 1bd6d557
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 不返回函数
void fatal_error(const char *msg) __attribute__((noreturn));

// 纯函数（无副作用，只依赖参数）
int square(int x) __attribute__((pure));

// 常量函数（更严格的纯函数，不读内存）
int max(int a, int b) __attribute__((const));

// 格式化检查
void log_message(const char *fmt, ...)
    __attribute__((format(printf, 1, 2)));

// 已废弃
void old_func(void) __attribute__((deprecated("use new_func")));

// 弱符号
void weak_func(void) __attribute__((weak));

// 构造函数/析构函数（启动/退出时自动调用）
void init(void) __attribute__((constructor));
void cleanup(void) __attribute__((destructor));

// 指定对齐
void *aligned_alloc(void) __attribute__((alloc_align(1)));
