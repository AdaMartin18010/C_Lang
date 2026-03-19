/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 2725
 * Language: c
 * Block ID: a5d792b0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*========================================
 * 问题：伪共享（False Sharing）
 *
 * CPU缓存行通常为64字节
 * 不同CPU核心上的线程修改同一缓存行的不同变量
 * 导致缓存无效化，性能下降
 *========================================*/

/* 问题代码：性能杀手 */
typedef struct {
    atomic_int counter0;  // CPU0频繁修改
    atomic_int counter1;  // CPU1频繁修改
    atomic_int counter2;  // CPU2频繁修改
    atomic_int counter3;  // CPU3频繁修改
    // 这4个int可能在同一缓存行（64字节）
} false_sharing_t;

/* 修复方案：缓存行填充 */
#define CACHE_LINE_SIZE 64

typedef struct {
    atomic_int counter;
    char padding[CACHE_LINE_SIZE - sizeof(atomic_int)];
} padded_counter_t;

typedef struct {
    padded_counter_t counters[4];  // 每个在独立缓存行
} no_false_sharing_t;

/* 实际应用：多线程计数器 */
void increment_counter(no_false_sharing_t *counters, int thread_id) {
    atomic_fetch_add(&counters->counters[thread_id].counter, 1);
}

/*========================================
 * 性能对比测试
 *========================================*/
#include <time.h>

void benchmark_false_sharing() {
    false_sharing_t fs;
    no_false_sharing_t nfs;

    atomic_init(&fs.counter0, 0);
    atomic_init(&fs.counter1, 0);
    atomic_init(&fs.counter2, 0);
    atomic_init(&fs.counter3, 0);

    // 测试无填充版本（慢）
    clock_t start = clock();
    for (int i = 0; i < 10000000; i++) {
        atomic_fetch_add(&fs.counter0, 1);
        atomic_fetch_add(&fs.counter1, 1);
        atomic_fetch_add(&fs.counter2, 1);
        atomic_fetch_add(&fs.counter3, 1);
    }
    printf("False sharing: %ld ticks\n", clock() - start);

    // 测试填充版本（快）
    start = clock();
    for (int i = 0; i < 10000000; i++) {
        atomic_fetch_add(&nfs.counters[0].counter, 1);
        atomic_fetch_add(&nfs.counters[1].counter, 1);
        atomic_fetch_add(&nfs.counters[2].counter, 1);
        atomic_fetch_add(&nfs.counters[3].counter, 1);
    }
    printf("No false sharing: %ld ticks\n", clock() - start);
}
