/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\02_Randomized_Algorithms.md
 * Line: 197
 * Language: c
 * Block ID: 70935870
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Intel RDRAND指令使用
#include <immintrin.h>
#include <stdbool.h>
#include <stdint.h>

// 使用RDRAND获取随机数
bool rdrand64(uint64_t *rand) {
    unsigned char ok;
    __asm__ volatile("rdrand %0; setc %1"
                     : "=r"(*rand), "=qm"(ok));
    return (bool)ok;
}

// 使用RDSEED获取熵种子
bool rdseed64(uint64_t *seed) {
    unsigned char ok;
    __asm__ volatile("rdseed %0; setc %1"
                     : "=r"(*seed), "=qm"(ok));
    return (bool)ok;
}

// 硬件RNG接口
typedef struct {
    enum { RNG_RDRAND, RNG_RDSEED, RNG_SOFTWARE } type;
    int retries;
} HardwareRNG;

uint64_t hardware_random(HardwareRNG *rng) {
    uint64_t value;
    for (int i = 0; i < rng->retries; i++) {
        if (rng->type == RNG_RDRAND && rdrand64(&value)) {
            return value;
        }
        if (rng->type == RNG_RDSEED && rdseed64(&value)) {
            return value;
        }
    }
    // 回退到软件
    return software_random();
}
