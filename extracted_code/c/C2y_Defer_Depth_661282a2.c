/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2001
 * Language: c
 * Block ID: 661282a2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 编译器生成的内部结构
typedef struct defer_entry {
    void (*func)(void *);
    void *arg;
    struct defer_entry *next;
} defer_entry;

// 每个包含defer的函数会有一个隐式的栈头
defer_entry *defer_stack = NULL;

// 压入defer
void defer_push(void (*func)(void *), void *arg) {
    defer_entry *entry = alloca(sizeof(defer_entry));
    entry->func = func;
    entry->arg = arg;
    entry->next = defer_stack;
    defer_stack = entry;
}

// 执行defer栈
void defer_execute_all(void) {
    while (defer_stack) {
        defer_entry *entry = defer_stack;
        defer_stack = entry->next;
        entry->func(entry->arg);
    }
}
