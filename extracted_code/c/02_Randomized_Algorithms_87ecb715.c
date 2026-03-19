/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\02_Randomized_Algorithms.md
 * Line: 384
 * Language: c
 * Block ID: 87ecb715
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// NIST测试套件（简化版）

typedef struct {
    const char *name;
    double (*test)(const uint8_t *, size_t);
    double threshold;
} NISTTest;

static NISTTest nist_tests[] = {
    {"Frequency", frequency_test, 0.01},
    {"Runs", /* runs_test_pvalue */ NULL, 0.01},
    {"Longest Run", NULL, 0.01},
    {"Binary Matrix Rank", NULL, 0.01},
    {"Discrete Fourier Transform", NULL, 0.01},
    // ... 更多测试
};

bool run_nist_tests(const uint8_t *data, size_t len) {
    int num_tests = sizeof(nist_tests) / sizeof(nist_tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        if (!nist_tests[i].test) continue;

        double p_value = nist_tests[i].test(data, len);
        bool pass = p_value >= nist_tests[i].threshold;

        printf("%s: p=%.4f %s\n",
               nist_tests[i].name, p_value,
               pass ? "PASS" : "FAIL");

        if (pass) passed++;
    }

    printf("Passed %d/%d tests\n", passed, num_tests);
    return passed == num_tests;
}
