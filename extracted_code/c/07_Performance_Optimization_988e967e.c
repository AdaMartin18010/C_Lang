/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\07_Performance_Optimization.md
 * Line: 481
 * Language: c
 * Block ID: 988e967e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    const char *name;
    void (*impl)(void);
} Variant;

void ab_test(Variant *variants, int num_variants, int iterations) {
    printf("A/B Test Results:\n");
    printf("%-20s %12s %12s %12s\n", "Variant", "Mean", "Min", "Max");

    for (int i = 0; i < num_variants; i++) {
        BenchmarkResult r = benchmark(variants[i].impl, iterations);
        printf("%-20s %12.2f %12.2f %12.2f\n",
               variants[i].name, r.mean, r.min, r.max);
    }
}
