/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\01_Language_Extensions.md
 * Line: 125
 * Language: c
 * Block ID: 2c0e1cf4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 基本内联汇编
__asm__ __volatile__ (
    "movl $42, %eax\n\t"
    "addl $1, %eax"
);

// 带输入输出
uint32_t result;
uint32_t input = 100;

__asm__ (
    "movl %[in], %[out]\n\t"
    "addl $1, %[out]"
    : [out] "=r" (result)      // 输出
    : [in] "r" (input)         // 输入
    : "memory"                 // 破坏描述
);

// 原子交换（x86）
static inline int atomic_xchg(int *ptr, int newval) {
    int oldval;
    __asm__ __volatile__ (
        "xchgl %[old], %[ptr]"
        : [old] "=r" (oldval), [ptr] "+m" (*ptr)
        : "0" (newval)
        : "memory"
    );
    return oldval;
}

// 内存屏障
#define memory_barrier() __asm__ __volatile__ ("" ::: "memory")

// CPUID
static inline void cpuid(uint32_t *eax, uint32_t *ebx,
                         uint32_t *ecx, uint32_t *edx) {
    __asm__ (
        "cpuid"
        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        : "a" (*eax)
    );
}
