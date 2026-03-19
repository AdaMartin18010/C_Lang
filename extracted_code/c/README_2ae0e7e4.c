/*
 * Auto-generated from: 07_Modern_Toolchain\12_Modern_C_Standards\README.md
 * Line: 490
 * Language: c
 * Block ID: 2ae0e7e4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 可移植性头文件 */

/* 线程支持包装 */
#if __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
    #include <threads.h>
    #define HAS_C11_THREADS 1
#else
    #include <pthread.h>
    #define thrd_t pthread_t
    #define mtx_t pthread_mutex_t
    #define thrd_create(t, f, a) pthread_create(t, NULL, f, a)
    #define thrd_join(t, r) pthread_join(t, r)
    #define mtx_init(m, t) pthread_mutex_init(m, NULL)
    #define mtx_lock(m) pthread_mutex_lock(m)
    #define mtx_unlock(m) pthread_mutex_unlock(m)
#endif

/* 原子操作包装 */
#if __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_ATOMICS__)
    #include <stdatomic.h>
    #define ATOMIC_INT _Atomic int
    #define atomic_inc(x) atomic_fetch_add(x, 1)
#else
    /* 使用 GCC 内建 */
    #define ATOMIC_INT int
    #define atomic_inc(x) __sync_fetch_and_add(x, 1)
#endif

/* 静态断言包装 */
#if __STDC_VERSION__ >= 201112L
    #define STATIC_ASSERT(expr, msg) _Static_assert(expr, msg)
#else
    #define STATIC_ASSERT(expr, msg) \
        typedef char STATIC_ASSERT_##__LINE__[(expr) ? 1 : -1]
#endif
