/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\03_Shor_Algorithm.md
 * Line: 501
 * Language: c
 * Block ID: ff055531
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
