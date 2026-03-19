/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\01_Rocket_Chip_Case_Study.md
 * Line: 878
 * Language: c
 * Block ID: 10e0c8f2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 内存带宽测试代码
void bandwidth_test(void) {
    const size_t size = 64 * 1024 * 1024;  // 64MB
    uint64_t *src = aligned_alloc(64, size);
    uint64_t *dst = aligned_alloc(64, size);

    // 初始化
    for (size_t i = 0; i < size / 8; i++) {
        src[i] = i;
    }

    PERF_START();

    // 顺序读写
    for (size_t iter = 0; iter < 10; iter++) {
        for (size_t i = 0; i < size / 8; i += 8) {
            dst[i] = src[i];
            dst[i+1] = src[i+1];
            dst[i+2] = src[i+2];
            dst[i+3] = src[i+3];
            dst[i+4] = src[i+4];
            dst[i+5] = src[i+5];
            dst[i+6] = src[i+6];
            dst[i+7] = src[i+7];
        }
    }

    PERF_END("Bandwidth Test");

    double bytes = (double)size * 10 * 2;  // 读+写
    double seconds = (double)(read_cycle() - _perf_start_cycle) / F_CPU;
    double bandwidth = bytes / seconds / 1e9;  // GB/s

    printf("Bandwidth: %.2f GB/s\n", bandwidth);
}
