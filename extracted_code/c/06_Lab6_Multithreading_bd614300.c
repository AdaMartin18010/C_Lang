/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\06_Lab6_Multithreading.md
 * Line: 484
 * Language: c
 * Block ID: bd614300
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用RISC-V原子指令

// 原子加
static inline int
atomic_add(int *ptr, int val)
{
    int result;
    __asm__ volatile(
        "amoadd.w %0, %2, (%1)"
        : "=r" (result)
        : "r" (ptr), "r" (val)
        : "memory"
    );
    return result;
}

// 测试并设置 (用于自旋锁)
static inline int
test_and_set(int *ptr, int val)
{
    int result;
    __asm__ volatile(
        "amoswap.w %0, %2, (%1)"
        : "=r" (result)
        : "r" (ptr), "r" (val)
        : "memory"
    );
    return result;
}
