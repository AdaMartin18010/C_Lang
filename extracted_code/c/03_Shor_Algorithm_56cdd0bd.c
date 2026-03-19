/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\03_Shor_Algorithm.md
 * Line: 729
 * Language: c
 * Block ID: 56cdd0bd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
