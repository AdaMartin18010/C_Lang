/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\04_Concurrency_Patterns.md
 * Line: 407
 * Language: c
 * Block ID: 0a0c2896
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

pthread_barrier_t barrier;

void init() {
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);
}

void *phase_work(void *arg) {
    // 阶段1
    do_phase1_work();
    pthread_barrier_wait(&barrier);  // 等待所有线程

    // 阶段2 (所有线程都完成了阶段1)
    do_phase2_work();
    pthread_barrier_wait(&barrier);

    return NULL;
}
