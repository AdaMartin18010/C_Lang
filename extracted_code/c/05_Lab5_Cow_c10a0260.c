/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\05_Lab5_Cow.md
 * Line: 630
 * Language: c
 * Block ID: c10a0260
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 测量fork性能
void benchmark_fork() {
    int start = uptime();

    // 分配大量内存
    sbrk(1024 * 1024 * 10);  // 10MB

    for (int i = 0; i < 100; i++) {
        int pid = fork();
        if (pid == 0) {
            exit(0);
        }
        wait(0);
    }

    int end = uptime();
    printf("100 forks took %d ticks\n", end - start);
}
