/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\README.md
 * Line: 276
 * Language: c
 * Block ID: f874f7d2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 1. CPUID指令封装
void cpuid(uint32_t func, uint32_t subfunc,
           uint32_t *eax, uint32_t *ebx,
           uint32_t *ecx, uint32_t *edx) {
    __asm__ __volatile__ (
        "cpuid"
        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        : "a" (func), "c" (subfunc)
    );
}

// 2. 内存屏障
#define memory_barrier() \
    __asm__ __volatile__ ("" ::: "memory")

// 3. 原子交换
static inline uint32_t atomic_swap(uint32_t *ptr, uint32_t newval) {
    uint32_t oldval;
    __asm__ __volatile__ (
        "xchg %0, %1"
        : "=r" (oldval), "+m" (*ptr)
        : "0" (newval)
        : "memory"
    );
    return oldval;
}

// 4. RDTSC读时间戳计数器
static inline uint64_t rdtsc(void) {
    uint32_t lo, hi;
    __asm__ __volatile__ (
        "rdtsc" : "=a" (lo), "=d" (hi)
    );
    return ((uint64_t)hi << 32) | lo;
}

// 使用RDTSC进行性能测量
void benchmark_example() {
    uint64_t start = rdtsc();

    // 被测代码
    volatile int sum = 0;
    for (int i = 0; i < 1000000; i++) {
        sum += i;
    }

    uint64_t end = rdtsc();
    printf("Cycles: %lu\n", end - start);
}
