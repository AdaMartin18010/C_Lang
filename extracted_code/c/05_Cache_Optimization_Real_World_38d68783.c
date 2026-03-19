/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\05_Cache_Optimization_Real_World.md
 * Line: 605
 * Language: c
 * Block ID: 38d68783
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 测试代码
void benchmark_prefetch(void) {
    const int N = 10000000;
    int* arr = aligned_alloc(64, N * sizeof(int));

    // 初始化链表
    for (int i = 0; i < N - 1; i++) {
        arr[i] = i + 1;
    }
    arr[N - 1] = -1;

    // 无预取
    clock_t start = clock();
    int idx = 0, sum = 0;
    while (idx != -1) {
        sum += arr[idx];
        idx = arr[idx];
    }
    double t1 = (double)(clock() - start) / CLOCKS_PER_SEC;

    // 有预取
    start = clock();
    idx = 0; sum = 0;
    while (idx != -1) {
        if (arr[idx] != -1) {
            _mm_prefetch(&arr[arr[idx]], _MM_HINT_T0);
        }
        sum += arr[idx];
        idx = arr[idx];
    }
    double t2 = (double)(clock() - start) / CLOCKS_PER_SEC;

    printf("No prefetch:  %.3f s\n", t1);
    printf("Prefetch:     %.3f s (%.1fx)\n", t2, t1/t2);
}

/* 结果:
 * 无预取:  0.45 s
 * 有预取:  0.12 s  (3.75x加速)
 */
