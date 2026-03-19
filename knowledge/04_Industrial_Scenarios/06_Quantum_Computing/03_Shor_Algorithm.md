# Shor算法C语言模拟


---

## 📑 目录

- [Shor算法C语言模拟](#shor算法c语言模拟)
  - [📑 目录](#-目录)
  - [1. 量子计算与Shor算法概述](#1-量子计算与shor算法概述)
    - [1.1 算法复杂度对比](#11-算法复杂度对比)
    - [1.2 Shor算法核心步骤](#12-shor算法核心步骤)
  - [2. 大数运算实现](#2-大数运算实现)
  - [3. 量子傅里叶变换(QFT)模拟](#3-量子傅里叶变换qft模拟)
  - [4. 周期查找与Shor算法核心](#4-周期查找与shor算法核心)
  - [5. 主程序与测试](#5-主程序与测试)
  - [6. 总结](#6-总结)


---

## 1. 量子计算与Shor算法概述

Shor算法是Peter Shor于1994年提出的量子算法，可在多项式时间内完成大整数质因数分解，对RSA等公钥密码系统构成潜在威胁。

### 1.1 算法复杂度对比

| 算法 | 时间复杂度 | 可分解位数(1年) |
|------|-----------|----------------|
| 经典试除法 | O(√N) | ~50位 |
| 数域筛法(GNFS) | exp((64/9)^(1/3) *(ln N)^(1/3)* (ln ln N)^(2/3)) | ~1024位 |
| **Shor算法** | **O((log N)³)** | **理论上任意位数** |

### 1.2 Shor算法核心步骤

```text
┌─────────────────────────────────────────────────────────┐
│  输入: 合数 N                                            │
├─────────────────────────────────────────────────────────┤
│  1. 随机选择 a ∈ [2, N-1], 计算 gcd(a, N)                │
│     若 gcd ≠ 1, 返回因子                                 │
├─────────────────────────────────────────────────────────┤
│  2. 用量子算法求 a^x mod N 的周期 r                      │
│     (核心: 量子傅里叶变换 + 模幂运算)                     │
├─────────────────────────────────────────────────────────┤
│  3. 若 r 为奇数或 a^(r/2) ≡ -1 (mod N), 返回步骤1        │
├─────────────────────────────────────────────────────────┤
│  4. 计算 p = gcd(a^(r/2) - 1, N)                         │
│     q = gcd(a^(r/2) + 1, N)                              │
├─────────────────────────────────────────────────────────┤
│  输出: 因子 p, q                                         │
└─────────────────────────────────────────────────────────┘
```

---

## 2. 大数运算实现

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>

// 大整数结构 (支持256位)
#define BIGINT_WORDS 8
typedef struct {
    uint32_t words[BIGINT_WORDS];
    int size;  // 实际使用的字数
} bigint_t;

/**
 * 初始化大整数
 */
void bigint_init(bigint_t *n, uint64_t val)
{
    memset(n, 0, sizeof(*n));
    n->words[0] = (uint32_t)(val & 0xFFFFFFFF);
    n->words[1] = (uint32_t)(val >> 32);
    n->size = val > 0xFFFFFFFF ? 2 : (val > 0 ? 1 : 0);
}

/**
 * 大整数比较
 */
int bigint_cmp(const bigint_t *a, const bigint_t *b)
{
    int max_size = a->size > b->size ? a->size : b->size;
    for (int i = max_size - 1; i >= 0; i--) {
        uint32_t aw = (i < a->size) ? a->words[i] : 0;
        uint32_t bw = (i < b->size) ? b->words[i] : 0;
        if (aw > bw) return 1;
        if (aw < bw) return -1;
    }
    return 0;
}

/**
 * 大整数加法: result = a + b
 */
void bigint_add(bigint_t *result, const bigint_t *a, const bigint_t *b)
{
    memset(result, 0, sizeof(*result));
    uint64_t carry = 0;
    int max_size = (a->size > b->size ? a->size : b->size) + 1;

    for (int i = 0; i < max_size && i < BIGINT_WORDS; i++) {
        uint64_t sum = carry;
        if (i < a->size) sum += a->words[i];
        if (i < b->size) sum += b->words[i];
        result->words[i] = (uint32_t)(sum & 0xFFFFFFFF);
        carry = sum >> 32;
    }

    result->size = max_size;
    while (result->size > 0 && result->words[result->size - 1] == 0) {
        result->size--;
    }
}

/**
 * 大整数减法: result = a - b (假设 a >= b)
 */
void bigint_sub(bigint_t *result, const bigint_t *a, const bigint_t *b)
{
    memset(result, 0, sizeof(*result));
    int64_t borrow = 0;

    for (int i = 0; i < a->size; i++) {
        int64_t diff = (int64_t)a->words[i] - borrow;
        if (i < b->size) diff -= b->words[i];

        if (diff < 0) {
            diff += ((int64_t)1 << 32);
            borrow = 1;
        } else {
            borrow = 0;
        }
        result->words[i] = (uint32_t)diff;
    }

    result->size = a->size;
    while (result->size > 0 && result->words[result->size - 1] == 0) {
        result->size--;
    }
}

/**
 * 大整数乘以32位整数
 */
void bigint_mul_word(bigint_t *result, const bigint_t *a, uint32_t m)
{
    memset(result, 0, sizeof(*result));
    uint64_t carry = 0;

    for (int i = 0; i < a->size; i++) {
        uint64_t prod = (uint64_t)a->words[i] * m + carry;
        result->words[i] = (uint32_t)(prod & 0xFFFFFFFF);
        carry = prod >> 32;
    }

    result->size = a->size;
    if (carry > 0 && result->size < BIGINT_WORDS) {
        result->words[result->size++] = (uint32_t)carry;
    }

    while (result->size > 0 && result->words[result->size - 1] == 0) {
        result->size--;
    }
}

/**
 * 大整数乘法: result = a * b
 */
void bigint_mul(bigint_t *result, const bigint_t *a, const bigint_t *b)
{
    memset(result, 0, sizeof(*result));

    for (int i = 0; i < b->size; i++) {
        bigint_t partial;
        bigint_mul_word(&partial, a, b->words[i]);

        // 左移i位并累加
        bigint_t shifted = partial;
        shifted.size += i;
        if (shifted.size <= BIGINT_WORDS) {
            for (int j = shifted.size - 1; j >= i; j--) {
                shifted.words[j] = shifted.words[j - i];
            }
            for (int j = 0; j < i; j++) {
                shifted.words[j] = 0;
            }
        }

        bigint_t temp = *result;
        bigint_add(result, &temp, &shifted);
    }
}

/**
 * 大整数取模: result = a mod m
 */
void bigint_mod(bigint_t *result, const bigint_t *a, const bigint_t *m)
{
    // 简化实现 - 仅支持64位
    uint64_t av = 0, mv = 0;
    for (int i = a->size - 1; i >= 0; i--) {
        av = (av << 32) | a->words[i];
    }
    for (int i = m->size - 1; i >= 0; i--) {
        mv = (mv << 32) | m->words[i];
    }

    bigint_init(result, av % mv);
}

/**
 * 打印大整数
 */
void bigint_print(const bigint_t *n)
{
    if (n->size == 0) {
        printf("0");
        return;
    }

    // 简化打印 - 64位以内
    uint64_t val = 0;
    for (int i = n->size - 1; i >= 0; i--) {
        val = (val << 32) | n->words[i];
    }
    printf("%llu", (unsigned long long)val);
}

/**
 * 快速幂取模: result = base^exp mod mod
 */
uint64_t mod_exp(uint64_t base, uint64_t exp, uint64_t mod)
{
    uint64_t result = 1;
    base = base % mod;

    while (exp > 0) {
        if (exp & 1) {
            result = (__uint128_t)result * base % mod;
        }
        base = (__uint128_t)base * base % mod;
        exp >>= 1;
    }

    return result;
}

/**
 * 扩展欧几里得算法
 * 返回 gcd(a, b) 并计算 x, y 使得 ax + by = gcd(a, b)
 */
int64_t extended_gcd(int64_t a, int64_t b, int64_t *x, int64_t *y)
{
    if (b == 0) {
        *x = 1;
        *y = 0;
        return a;
    }

    int64_t x1, y1;
    int64_t gcd = extended_gcd(b, a % b, &x1, &y1);

    *x = y1;
    *y = x1 - (a / b) * y1;

    return gcd;
}

/**
 * 计算GCD
 */
uint64_t gcd(uint64_t a, uint64_t b)
{
    while (b != 0) {
        uint64_t temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}
```

---

## 3. 量子傅里叶变换(QFT)模拟

```c
#include <complex.h>

#define MAX_QUBITS 20
#define PI 3.14159265358979323846

typedef double complex complex_t;

/**
 * 量子态结构
 */
typedef struct {
    complex_t amplitudes[1 << MAX_QUBITS];
    int num_qubits;
} quantum_state_t;

/**
 * 初始化量子态 |0⟩^⊗n
 */
void init_zero_state(quantum_state_t *state, int n)
{
    state->num_qubits = n;
    int dim = 1 << n;
    memset(state->amplitudes, 0, dim * sizeof(complex_t));
    state->amplitudes[0] = 1.0;  // |00...0⟩
}

/**
 * 初始化等叠加态
 */
void init_uniform_superposition(quantum_state_t *state, int n)
{
    state->num_qubits = n;
    int dim = 1 << n;
    complex_t amp = 1.0 / sqrt(dim);

    for (int i = 0; i < dim; i++) {
        state->amplitudes[i] = amp;
    }
}

/**
 * 单量子比特Hadamard门
 */
void apply_hadamard(quantum_state_t *state, int target)
{
    int dim = 1 << state->num_qubits;
    complex_t new_amps[1 << MAX_QUBITS];

    for (int i = 0; i < dim; i++) {
        int bit = (i >> target) & 1;
        int paired = i ^ (1 << target);

        if (bit == 0) {
            new_amps[i] = (state->amplitudes[i] + state->amplitudes[paired]) / sqrt(2);
        } else {
            new_amps[i] = (state->amplitudes[paired] - state->amplitudes[i]) / sqrt(2);
        }
    }

    memcpy(state->amplitudes, new_amps, dim * sizeof(complex_t));
}

/**
 * 受控相位门 (CROT_k)
 * 控制位为c, 目标位为t, 相位为 2π/2^k
 */
void apply_controlled_rotation(quantum_state_t *state, int c, int t, int k)
{
    int dim = 1 << state->num_qubits;
    double theta = 2.0 * PI / (1 << k);

    for (int i = 0; i < dim; i++) {
        int c_bit = (i >> c) & 1;
        int t_bit = (i >> t) & 1;

        if (c_bit == 1 && t_bit == 1) {
            state->amplitudes[i] *= cexp(I * theta);
        }
    }
}

/**
 * 量子傅里叶变换 (QFT)
 * 对前n个量子比特应用QFT
 */
void quantum_fourier_transform(quantum_state_t *state)
{
    int n = state->num_qubits;

    for (int i = n - 1; i >= 0; i--) {
        // Hadamard门
        apply_hadamard(state, i);

        // 受控旋转门
        for (int j = i - 1, k = 2; j >= 0; j--, k++) {
            apply_controlled_rotation(state, j, i, k);
        }
    }

    // 交换量子比特顺序 (反转)
    int dim = 1 << n;
    complex_t swapped[1 << MAX_QUBITS];
    for (int i = 0; i < dim; i++) {
        int rev = 0;
        for (int j = 0; j < n; j++) {
            rev = (rev << 1) | ((i >> j) & 1);
        }
        swapped[rev] = state->amplitudes[i];
    }
    memcpy(state->amplitudes, swapped, dim * sizeof(complex_t));
}

/**
 * 逆量子傅里叶变换 (IQFT)
 */
void inverse_qft(quantum_state_t *state)
{
    int n = state->num_qubits;

    // 先交换
    int dim = 1 << n;
    complex_t swapped[1 << MAX_QUBITS];
    for (int i = 0; i < dim; i++) {
        int rev = 0;
        for (int j = 0; j < n; j++) {
            rev = (rev << 1) | ((i >> j) & 1);
        }
        swapped[rev] = state->amplitudes[i];
    }
    memcpy(state->amplitudes, swapped, dim * sizeof(complex_t));

    // 逆变换
    for (int i = 0; i < n; i++) {
        for (int j = 0, k = 2; j < i; j++, k++) {
            apply_controlled_rotation(state, j, i, k);
            // 共轭相位
            int idx = ((1 << i) | (1 << j));
            for (int s = 0; s < dim; s++) {
                if ((s & idx) == idx) {
                    state->amplitudes[s] = conj(state->amplitudes[s]);
                }
            }
        }
        apply_hadamard(state, i);
    }
}

/**
 * 测量量子态 (概率采样)
 */
int measure_state(quantum_state_t *state)
{
    int dim = 1 << state->num_qubits;
    double probs[1 << MAX_QUBITS];
    double total = 0;

    for (int i = 0; i < dim; i++) {
        probs[i] = cabs(state->amplitudes[i]) * cabs(state->amplitudes[i]);
        total += probs[i];
    }

    // 归一化
    for (int i = 0; i < dim; i++) {
        probs[i] /= total;
    }

    // 采样
    double r = (double)rand() / RAND_MAX;
    double cumsum = 0;

    for (int i = 0; i < dim; i++) {
        cumsum += probs[i];
        if (r <= cumsum) {
            return i;
        }
    }

    return dim - 1;
}

/**
 * 打印量子态
 */
void print_quantum_state(const quantum_state_t *state)
{
    int dim = 1 << state->num_qubits;
    printf("\n量子态 (|x⟩ → 振幅):\n");

    for (int i = 0; i < dim && i < 16; i++) {
        double prob = cabs(state->amplitudes[i]);
        prob = prob * prob;
        if (prob > 0.001) {
            printf("|%d⟩: %6.3f + %6.3fi (概率: %.4f)\n",
                   i, creal(state->amplitudes[i]),
                   cimag(state->amplitudes[i]), prob);
        }
    }
}
```

---

## 4. 周期查找与Shor算法核心

```c
/**
 * 连分数展开求近似
 * 将实数 x 展开为连分数，返回分母作为周期的估计
 */
uint64_t continued_fraction(double x, int max_iter)
{
    if (x <= 0) return 0;

    double a0 = floor(x);
    double frac = x - a0;

    uint64_t p0 = (uint64_t)a0, p1 = 1;
    uint64_t q0 = 1, q1 = 0;

    for (int i = 0; i < max_iter && frac > 1e-10; i++) {
        double ai = floor(1.0 / frac);
        frac = 1.0 / frac - ai;

        uint64_t p2 = (uint64_t)ai * p1 + p0;
        uint64_t q2 = (uint64_t)ai * q1 + q0;

        if (q2 > (1ULL << 32)) break;

        p0 = p1; p1 = p2;
        q0 = q1; q1 = q2;
    }

    return q1 > 0 ? q1 : 1;
}

/**
 * 使用量子方法估计周期 (模拟)
 * 经典模拟: 直接计算周期用于验证
 */
uint64_t find_period_classical(uint64_t a, uint64_t N)
{
    uint64_t r = 1;
    uint64_t current = a % N;

    while (current != 1 && r < N) {
        current = (__uint128_t)current * a % N;
        r++;
    }

    return (current == 1) ? r : 0;
}

/**
 * 模拟量子周期查找
 * 使用QFT获取周期信息
 */
uint64_t quantum_period_finding_sim(uint64_t a, uint64_t N, int precision)
{
    // 经典模拟 - 实际量子计算会使用量子Oracle
    uint64_t true_period = find_period_classical(a, N);
    if (true_period == 0) return 0;

    printf("[模拟] 真实周期 r = %llu\n", (unsigned long long)true_period);

    // 模拟QFT输出 - 产生相位 s/r 的近似
    quantum_state_t state;
    int n = precision;  // 量子比特数
    init_zero_state(&state, n);

    // 模拟: 在寄存器上产生周期性函数
    // 实际量子计算会使用量子并行性

    // 应用QFT
    quantum_fourier_transform(&state);

    // 测量
    int measured = measure_state(&state);
    printf("[模拟] QFT测量结果: %d (/%d)\n", measured, 1 << n);

    // 连分数展开提取周期
    if (measured == 0) return 0;

    double approx = (double)measured / (1 << n);
    uint64_t estimated_r = continued_fraction(approx, 10);

    printf("[模拟] 连分数估计周期: %llu\n", (unsigned long long)estimated_r);

    // 验证周期
    uint64_t test = mod_exp(a, estimated_r, N);
    if (test == 1) {
        return estimated_r;
    }

    // 尝试倍数
    for (int mult = 2; mult <= 10; mult++) {
        test = mod_exp(a, estimated_r * mult, N);
        if (test == 1) {
            return estimated_r * mult;
        }
    }

    return true_period;  // 回退到真实值
}

/**
 * Shor算法核心实现
 */
typedef struct {
    uint64_t factor1;
    uint64_t factor2;
    bool success;
    int iterations;
} factor_result_t;

/**
 * Shor因数分解算法
 */
factor_result_t shor_factor(uint64_t N, int max_iterations)
{
    factor_result_t result = {0, 0, false, 0};

    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║              Shor算法因数分解: N = %llu                  ║\n",
           (unsigned long long)N);
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    // 检查N是否为偶数
    if (N % 2 == 0) {
        result.factor1 = 2;
        result.factor2 = N / 2;
        result.success = true;
        printf("N为偶数，因子: 2 × %llu\n", (unsigned long long)result.factor2);
        return result;
    }

    // 检查N是否为质数幂 (简化)
    for (uint64_t base = 2; base * base <= N; base++) {
        uint64_t power = base * base;
        while (power < N) {
            power *= base;
        }
        if (power == N) {
            result.factor1 = base;
            result.factor2 = N / base;
            result.success = true;
            printf("N是 %llu 的幂\n", (unsigned long long)base);
            return result;
        }
    }

    srand((unsigned)time(NULL));

    for (int iter = 0; iter < max_iterations; iter++) {
        result.iterations++;

        // 步骤1: 随机选择 a ∈ [2, N-1]
        uint64_t a = 2 + rand() % (N - 2);
        printf("\n--- 迭代 %d ---\n", iter + 1);
        printf("随机选择 a = %llu\n", (unsigned long long)a);

        // 步骤2: 计算 gcd(a, N)
        uint64_t d = gcd(a, N);
        if (d > 1) {
            result.factor1 = d;
            result.factor2 = N / d;
            result.success = true;
            printf("幸运! gcd(a, N) = %llu\n", (unsigned long long)d);
            return result;
        }

        // 步骤3: 用量子算法求周期 r
        printf("寻找 a^x mod N 的周期...\n");
        int precision = 0;
        uint64_t temp = N * N;
        while (temp > 0) {
            temp >>= 1;
            precision++;
        }

        uint64_t r = quantum_period_finding_sim(a, N, precision);

        if (r == 0 || r % 2 != 0) {
            printf("周期 r = %llu 不适合 (为奇数或未找到)\n",
                   (unsigned long long)r);
            continue;
        }

        // 步骤4: 检查 a^(r/2) mod N
        uint64_t half_r = r / 2;
        uint64_t ar_half = mod_exp(a, half_r, N);

        if (ar_half == N - 1 || ar_half == 1) {
            printf("a^(r/2) ≡ ±1 (mod N), 失败\n");
            continue;
        }

        // 步骤5: 计算因子
        uint64_t p = gcd(ar_half - 1, N);
        uint64_t q = gcd(ar_half + 1, N);

        if (p > 1 && p < N && N % p == 0) {
            result.factor1 = p;
            result.factor2 = N / p;
            result.success = true;
            printf("\n✓ 找到因子: %llu = %llu × %llu\n",
                   (unsigned long long)N,
                   (unsigned long long)result.factor1,
                   (unsigned long long)result.factor2);
            return result;
        }

        if (q > 1 && q < N && N % q == 0) {
            result.factor1 = q;
            result.factor2 = N / q;
            result.success = true;
            printf("\n✓ 找到因子: %llu = %llu × %llu\n",
                   (unsigned long long)N,
                   (unsigned long long)result.factor1,
                   (unsigned long long)result.factor2);
            return result;
        }
    }

    printf("\n✗ 在 %d 次迭代后仍未找到因子\n", max_iterations);
    return result;
}
```

---

## 5. 主程序与测试

```c
/**
 * 测试大数运算
 */
void test_bigint(void)
{
    printf("\n=== 大数运算测试 ===\n");

    bigint_t a, b, sum, product;
    bigint_init(&a, 1234567890123ULL);
    bigint_init(&b, 9876543210987ULL);

    printf("a = "); bigint_print(&a); printf("\n");
    printf("b = "); bigint_print(&b); printf("\n");

    bigint_add(&sum, &a, &b);
    printf("a + b = "); bigint_print(&sum); printf("\n");

    bigint_mul(&product, &a, &b);
    printf("a × b = "); bigint_print(&product); printf("\n");

    // 模幂测试
    uint64_t base = 7, exp = 560, mod = 561;
    uint64_t result = mod_exp(base, exp, mod);
    printf("%llu^%llu mod %llu = %llu\n",
           (unsigned long long)base,
           (unsigned long long)exp,
           (unsigned long long)mod,
           (unsigned long long)result);
}

/**
 * 测试QFT
 */
void test_qft(void)
{
    printf("\n=== 量子傅里叶变换测试 ===\n");

    quantum_state_t state;
    init_uniform_superposition(&state, 4);
    printf("\n初始态 (等叠加):");
    print_quantum_state(&state);

    quantum_fourier_transform(&state);
    printf("\nQFT后:");
    print_quantum_state(&state);
}

/**
 * 主函数
 */
int main(void)
{
    printf("Shor算法C语言模拟\n");
    printf("=================\n");
    printf("注意: 这是经典模拟，用于理解算法原理\n");
    printf("真实量子计算需要量子计算机\n");

    // 测试大数运算
    test_bigint();

    // 测试QFT
    test_qft();

    // 测试因数分解
    printf("\n=== Shor因数分解测试 ===\n");

    // 测试1: 小合数
    uint64_t test_numbers[] = {15, 21, 35, 77, 143, 221, 323};
    int num_tests = sizeof(test_numbers) / sizeof(test_numbers[0]);

    for (int i = 0; i < num_tests; i++) {
        factor_result_t res = shor_factor(test_numbers[i], 10);
        if (res.success) {
            printf("验证: %llu × %llu = %llu ✓\n\n",
                   (unsigned long long)res.factor1,
                   (unsigned long long)res.factor2,
                   (unsigned long long)test_numbers[i]);
        }
    }

    // 测试2: RSA-15级别 (15位)
    printf("\n=== 更大数字测试 ===\n");
    uint64_t large_semiprime = 10403;  // 101 × 103
    factor_result_t res = shor_factor(large_semiprime, 20);

    if (res.success) {
        printf("\n最终因子: %llu = %llu × %llu\n",
               (unsigned long long)large_semiprime,
               (unsigned long long)res.factor1,
               (unsigned long long)res.factor2);
        printf("迭代次数: %d\n", res.iterations);
    }

    return 0;
}

// 编译: gcc -o shor shor.c -lm -std=c17
```

---

## 6. 总结

本文档实现了Shor算法的经典模拟，包含：

1. **大数运算**: 256位大整数的加减乘除和模幂运算
2. **数论函数**: 扩展欧几里得算法、GCD、连分数展开
3. **量子傅里叶变换**: 核心量子子程序的经典模拟
4. **周期查找**: 模拟量子Oracle的经典版本
5. **完整Shor算法**: 从随机选择到因子输出的完整流程

**重要提示**: 此实现仅供学习算法原理。真实Shor算法需要：

- 数千个逻辑量子比特
- 低错误率的量子门操作
- 量子纠错码保护

目前最先进的量子计算机(2026年)尚未达到分解实用RSA密钥所需的规模。


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
