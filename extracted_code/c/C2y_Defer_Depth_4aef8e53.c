/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2251
 * Language: c
 * Block ID: 4aef8e53
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// GCC生成的内部数据结构
typedef struct {
    void (*destructor)(void *);
    void *object;
} gcc_defer_entry;

// 使用线程局部存储的defer栈
__thread gcc_defer_entry *gcc_defer_stack = NULL;
__thread int gcc_defer_stack_size = 0;
