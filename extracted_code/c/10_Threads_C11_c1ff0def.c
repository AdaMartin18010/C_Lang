/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\10_Threads_C11.md
 * Line: 1940
 * Language: c
 * Block ID: c1ff0def
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <string.h>

/* 调试模式下的锁追踪 */
#ifdef DEBUG_LOCKS
    #define MAX_LOCKS 64
    #define MAX_THREADS 16

    typedef struct {
        mtx_t* lock;
        const char* name;
        thrd_t owner;
        struct timespec acquired_at;
    } LockInfo;

    static LockInfo g_locks[MAX_LOCKS];
    static atomic_int g_lock_count = 0;
    static mtx_t g_debug_mutex;

    void debug_lock_init(void) {
        mtx_init(&g_debug_mutex, mtx_plain);
    }

    void debug_lock_register(mtx_t* lock, const char* name) {
        mtx_lock(&g_debug_mutex);
        int idx = atomic_fetch_add(&g_lock_count, 1);
        if (idx < MAX_LOCKS) {
            g_locks[idx].lock = lock;
            g_locks[idx].name = name;
            g_locks[idx].owner = 0;
        }
        mtx_unlock(&g_debug_mutex);
    }

    void debug_lock_acquired(mtx_t* lock, thrd_t owner) {
        mtx_lock(&g_debug_mutex);
        for (int i = 0; i < atomic_load(&g_lock_count); i++) {
            if (g_locks[i].lock == lock) {
                /* 检查是否已持有 */
                if (g_locks[i].owner != 0) {
                    fprintf(stderr, "\n[DEADLOCK WARNING] %s already held by "
                            "thread!\n", g_locks[i].name);
                }
                g_locks[i].owner = owner;
                timespec_get(&g_locks[i].acquired_at, TIME_UTC);
                break;
            }
        }
        mtx_unlock(&g_debug_mutex);
    }

    void debug_lock_released(mtx_t* lock) {
        mtx_lock(&g_debug_mutex);
        for (int i = 0; i < atomic_load(&g_lock_count); i++) {
            if (g_locks[i].lock == lock) {
                g_locks[i].owner = 0;
                break;
            }
        }
        mtx_unlock(&g_debug_mutex);
    }

    void debug_print_lock_status(void) {
        mtx_lock(&g_debug_mutex);
        printf("\n=== Lock Status ===\n");
        struct timespec now;
        timespec_get(&now, TIME_UTC);

        for (int i = 0; i < atomic_load(&g_lock_count); i++) {
            if (g_locks[i].owner != 0) {
                double held_ms = (now.tv_sec - g_locks[i].acquired_at.tv_sec)
                                 * 1000.0 +
                    (now.tv_nsec - g_locks[i].acquired_at.tv_nsec) / 1000000.0;
                printf("  %s: held by thread for %.1f ms\n",
                       g_locks[i].name, held_ms);
            } else {
                printf("  %s: unlocked\n", g_locks[i].name);
            }
        }
        mtx_unlock(&g_debug_mutex);
    }
#else
    #define debug_lock_init() ((void)0)
    #define debug_lock_register(lock, name) ((void)0)
    #define debug_lock_acquired(lock, owner) ((void)0)
    #define debug_lock_released(lock) ((void)0)
    #define debug_print_lock_status() ((void)0)
#endif
