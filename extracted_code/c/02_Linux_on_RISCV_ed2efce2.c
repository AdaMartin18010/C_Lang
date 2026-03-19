/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\02_Linux_on_RISCV.md
 * Line: 916
 * Language: c
 * Block ID: ed2efce2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 实时线程设置
void setup_realtime_thread(pthread_t thread, int cpu) {
    struct sched_param param;
    cpu_set_t cpuset;

    // 设置SCHED_FIFO调度策略
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(thread, SCHED_FIFO, &param);

    // 绑定到特定CPU
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    pthread_setaffinity_np(thread, sizeof(cpuset), &cpuset);

    // 内存锁定
    mlockall(MCL_CURRENT | MCL_FUTURE);
}

// 测试结果
void latency_test(void) {
    struct timespec start, end;
    long long min = LLONG_MAX, max = 0, sum = 0;

    for (int i = 0; i < 100000; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);

        // 触发中断
        trigger_irq();

        // 等待响应
        wait_for_response();

        clock_gettime(CLOCK_MONOTONIC, &end);

        long long latency = (end.tv_sec - start.tv_sec) * 1000000000LL +
                           (end.tv_nsec - start.tv_nsec);

        min = min(min, latency);
        max = max(max, latency);
        sum += latency;
    }

    printf("Latency: min=%lld ns, max=%lld ns, avg=%lld ns\n",
           min, max, sum / 100000);
}
