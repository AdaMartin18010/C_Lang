/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\05_Thread_Pool_Patterns.md
 * Line: 401
 * Language: c
 * Block ID: eb948c0d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 根据CPU负载动态调整 */
typedef struct {
    atomic_int load[MAX_WORKERS];  /* 每个工作线程的负载 */
} load_balancer_t;

int select_worker(load_balancer_t* lb, int num_workers) {
    /* 选择负载最低的工作者 */
    int min_load = INT_MAX;
    int selected = 0;

    for (int i = 0; i < num_workers; i++) {
        int load = atomic_load(&lb->load[i]);
        if (load < min_load) {
            min_load = load;
            selected = i;
        }
    }

    atomic_fetch_add(&lb->load[selected], 1);
    return selected;
}
