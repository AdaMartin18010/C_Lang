/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\README.md
 * Line: 330
 * Language: c
 * Block ID: 18e78fa4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 标记为volatile防止编译器优化删除
// 使用"memory"约束防止指令重排序

// 正确的同步原语实现
static inline void acquire_lock(int *lock) {
    __asm__ __volatile__ (
        "1: movl $1, %%eax\n\t"
        "xchg %%eax, %0\n\t"
        "test %%eax, %%eax\n\t"
        "jnz 1b"
        : "+m" (*lock)
        :
        : "eax", "memory"
    );
}

static inline void release_lock(int *lock) {
    __asm__ __volatile__ (
        "movl $0, %0"
        : "=m" (*lock)
        :
        : "memory"
    );
}
