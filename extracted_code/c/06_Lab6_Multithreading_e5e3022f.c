/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\06_Lab6_Multithreading.md
 * Line: 609
 * Language: c
 * Block ID: e5e3022f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 计算加速比
void speedup_test() {
    int start, end;

    // 单线程版本
    start = uptime();
    compute_single_thread();
    end = uptime();
    int single_time = end - start;

    // 多线程版本
    start = uptime();
    compute_multi_thread(NUM_THREADS);
    end = uptime();
    int multi_time = end - start;

    printf("Single thread: %d ticks\n", single_time);
    printf("Multi thread: %d ticks\n", multi_time);
    printf("Speedup: %.2fx\n", (float)single_time / multi_time);
}
