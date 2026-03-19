/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\09_MISRA_C_2023_Rules_9.md
 * Line: 314
 * Language: c
 * Block ID: f6b6ba2a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 互斥锁保护 */
#include <pthread.h>

static int shared_counter = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void increment_safe(void)
{
    pthread_mutex_lock(&counter_mutex);
    shared_counter++;
    pthread_mutex_unlock(&counter_mutex);
}

/* ✅ 合规 - 原子操作 */
#include <stdatomic.h>

static _Atomic(int) atomic_counter = 0;

void increment_atomic(void)
{
    atomic_fetch_add(&atomic_counter, 1, memory_order_relaxed);
}

/* ✅ 合规 - 读写锁 */
static pthread_rwlock_t data_rwlock = PTHREAD_RWLOCK_INITIALIZER;
static struct data shared_data;

void read_data(struct data *out)
{
    pthread_rwlock_rdlock(&data_rwlock);
    *out = shared_data;
    pthread_rwlock_unlock(&data_rwlock);
}

void write_data(const struct data *in)
{
    pthread_rwlock_wrlock(&data_rwlock);
    shared_data = *in;
    pthread_rwlock_unlock(&data_rwlock);
}
