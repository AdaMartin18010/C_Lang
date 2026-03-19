/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\02_Randomized_Algorithms.md
 * Line: 136
 * Language: c
 * Block ID: f4216557
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 蒙特卡洛算法示例：Miller-Rabin素性测试
#include <stdint.h>
#include <stdbool.h>

// 模幂运算
uint64_t mod_pow(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) result = (result * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return result;
}

// Miller-Rabin测试
// 错误概率 <= 4^(-k)
bool miller_rabin(uint64_t n, int k) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0) return false;

    // 写成 n-1 = d * 2^s
    uint64_t d = n - 1;
    int s = 0;
    while ((d & 1) == 0) {
        d >>= 1;
        s++;
    }

    // k轮测试
    for (int i = 0; i < k; i++) {
        uint64_t a = 2 + rand() % (n - 3);  // [2, n-2]
        uint64_t x = mod_pow(a, d, n);

        if (x == 1 || x == n - 1) continue;

        bool composite = true;
        for (int r = 1; r < s; r++) {
            x = mod_pow(x, 2, n);
            if (x == n - 1) {
                composite = false;
                break;
            }
        }

        if (composite) return false;  // 确定是合数
    }

    return true;  // 可能是素数（概率 > 1 - 4^(-k))
}

// k=10时，错误概率 < 0.000001
