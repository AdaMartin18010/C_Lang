/*
 * Auto-generated from: 01_Core_Knowledge_System\03_Construction_Layer\03_Modularization_Linking.md
 * Line: 213
 * Language: c
 * Block ID: 569e1963
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// extern：声明在其他地方定义
extern int global_counter;  // 只声明，不定义

// static：内部链接或存储期延长
static int file_scope_var;  // 内部链接
void func(void) {
    static int persistent = 0;  // 静态存储期，只初始化一次
    persistent++;
}

// auto：自动存储期（默认，可省略）
void func2(void) {
    auto int local = 5;  // 等价于 int local = 5;
}

// register：建议存储在寄存器（C11已废弃含义）
void func3(void) {
    register int i;  // 现代编译器忽略
    for (i = 0; i < 100; i++) { }
    // &i 不允许（无法取地址）
}

// _Thread_local：线程存储期（C11）
_Thread_local int thread_local_var;  // 每个线程独立
