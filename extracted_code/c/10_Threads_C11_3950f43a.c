/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\10_Threads_C11.md
 * Line: 2036
 * Language: c
 * Block ID: 3950f43a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 线程状态跟踪 */
typedef enum {
    THREAD_STATE_IDLE,
    THREAD_STATE_RUNNING,
    THREAD_STATE_BLOCKED,
    THREAD_STATE_TERMINATED
} ThreadState;

typedef struct {
    thrd_t id;
    const char* name;
    _Atomic(ThreadState) state;
    struct timespec state_changed;
    void* current_task;
} ThreadDebugInfo;

#define MAX_TRACKED_THREADS 32
static ThreadDebugInfo g_thread_info[MAX_TRACKED_THREADS];
static atomic_int g_thread_count = 0;

void thread_register(const char* name) {
    int idx = atomic_fetch_add(&g_thread_count, 1);
    if (idx < MAX_TRACKED_THREADS) {
        g_thread_info[idx].id = thrd_current();
        g_thread_info[idx].name = name;
        atomic_store(&g_thread_info[idx].state, THREAD_STATE_IDLE);
        timespec_get(&g_thread_info[idx].state_changed, TIME_UTC);
    }
}

void thread_set_state(ThreadState state) {
    thrd_t self = thrd_current();
    for (int i = 0; i < atomic_load(&g_thread_count); i++) {
        if (thrd_equal(g_thread_info[i].id, self)) {
            atomic_store(&g_thread_info[i].state, state);
            timespec_get(&g_thread_info[i].state_changed, TIME_UTC);
            break;
        }
    }
}

void print_thread_status(void) {
    printf("\n=== Thread Status ===\n");
    struct timespec now;
    timespec_get(&now, TIME_UTC);

    const char* state_names[] = {"IDLE", "RUNNING", "BLOCKED", "TERMINATED"};

    for (int i = 0; i < atomic_load(&g_thread_count); i++) {
        ThreadState state = atomic_load(&g_thread_info[i].state);
        double in_state_ms = (now.tv_sec - g_thread_info[i].state_changed.tv_sec)
                             * 1000.0 +
            (now.tv_nsec - g_thread_info[i].state_changed.tv_nsec) / 1000000.0;
        printf("  [%s] %s for %.1f ms\n",
               g_thread_info[i].name,
               state_names[state],
               in_state_ms);
    }
}
