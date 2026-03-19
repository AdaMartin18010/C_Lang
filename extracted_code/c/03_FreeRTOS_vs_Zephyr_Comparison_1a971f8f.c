/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 1257
 * Language: c
 * Block ID: 1a971f8f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Zephyr RTOS 信号量完整示例
 * 文件: zephyr_semaphore_example.c
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(semaphore_example, LOG_LEVEL_DBG);

/* 定义信号量 */
static K_SEM_DEFINE(binary_sem, 0, 1);      /* 二值信号量 */
static K_SEM_DEFINE(counting_sem, 0, 10);   /* 计数信号量 */
static K_MUTEX_DEFINE(resource_mutex);      /* 互斥锁 */

/* 共享资源 */
static volatile uint32_t shared_resource = 0;

/*
 * 二值信号量线程
 */
static void binary_sem_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    LOG_INF("[BinarySemThread] Waiting for semaphore...");

    while (1) {
        /* 等待信号量 - 支持超时 */
        int ret = k_sem_take(&binary_sem, K_MSEC(1000));

        if (ret == 0) {
            LOG_INF("[BinarySemThread] Semaphore received, processing...");

            /* 执行操作 */
            k_sleep(K_MSEC(100));

            LOG_INF("[BinarySemThread] Processing complete");
        } else {
            LOG_WRN("[BinarySemThread] Timeout waiting for semaphore");
        }
    }
}

/*
 * 计数信号量生产者线程
 */
static void counting_producer(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    uint32_t produced_count = 0;

    while (1) {
        k_sleep(K_MSEC(200));
        produced_count++;

        /* 释放计数信号量 */
        int ret = k_sem_give(&counting_sem);

        if (ret == 0) {
            LOG_INF("[Producer] Produced item #%u, count=%u",
                    produced_count, k_sem_count_get(&counting_sem));
        } else {
            LOG_WRN("[Producer] Semaphore full, item dropped");
        }
    }
}

/*
 * 计数信号量消费者线程
 */
static void counting_consumer(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    uint32_t consumed_count = 0;

    while (1) {
        /* 等待可用资源 */
        k_sem_take(&counting_sem, K_FOREVER);

        consumed_count++;
        LOG_INF("[Consumer] Consumed item #%u", consumed_count);

        /* 处理资源 */
        k_sleep(K_MSEC(500));
    }
}

/*
 * 互斥锁保护线程A
 */
static void mutex_thread_a(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (1) {
        /* 获取互斥锁 - 带超时 */
        int ret = k_mutex_lock(&resource_mutex, K_MSEC(100));

        if (ret == 0) {
            LOG_INF("[MutexThreadA] Acquired mutex");

            /* 访问共享资源 */
            uint32_t temp = shared_resource;
            k_sleep(K_MSEC(10));
            shared_resource = temp + 1;
            LOG_INF("[MutexThreadA] Resource incremented to %u", shared_resource);

            /* 释放互斥锁 */
            k_mutex_unlock(&resource_mutex);
            LOG_INF("[MutexThreadA] Released mutex");
        } else {
            LOG_WRN("[MutexThreadA] Failed to acquire mutex: %d", ret);
        }

        k_sleep(K_MSEC(200));
    }
}

/*
 * 互斥锁保护线程B
 */
static void mutex_thread_b(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (1) {
        /* 获取互斥锁 */
        k_mutex_lock(&resource_mutex, K_FOREVER);

        LOG_INF("[MutexThreadB] Acquired mutex");

        /* 访问共享资源 */
        shared_resource *= 2;
        LOG_INF("[MutexThreadB] Resource doubled to %u", shared_resource);

        k_mutex_unlock(&resource_mutex);
        LOG_INF("[MutexThreadB] Released mutex");

        k_sleep(K_MSEC(300));
    }
}

/*
 * 递归锁替代方案 - 使用计数器
 * Zephyr不直接支持递归互斥锁
 */
struct recursive_lock {
    struct k_mutex mutex;
    k_tid_t owner;
    int count;
};

static struct recursive_lock my_recursive_lock;

static void recursive_lock_init(struct recursive_lock *lock)
{
    k_mutex_init(&lock->mutex);
    lock->owner = NULL;
    lock->count = 0;
}

static int recursive_lock_acquire(struct recursive_lock *lock)
{
    k_tid_t current = k_current_get();

    if (lock->owner == current) {
        /* 已持有锁，增加计数 */
        lock->count++;
        return 0;
    }

    /* 尝试获取锁 */
    int ret = k_mutex_lock(&lock->mutex, K_FOREVER);
    if (ret == 0) {
        lock->owner = current;
        lock->count = 1;
    }
    return ret;
}

static int recursive_lock_release(struct recursive_lock *lock)
{
    k_tid_t current = k_current_get();

    if (lock->owner != current) {
        return -EINVAL;  /* 不持有该锁 */
    }

    lock->count--;

    if (lock->count == 0) {
        lock->owner = NULL;
        return k_mutex_unlock(&lock->mutex);
    }

    return 0;
}

/* 递归函数演示 */
static void recursive_operation(int depth, struct recursive_lock *lock)
{
    if (depth <= 0) {
        return;
    }

    recursive_lock_acquire(lock);
    LOG_INF("[Recursive] Depth %d: Acquired lock", depth);

    recursive_operation(depth - 1, lock);

    LOG_INF("[Recursive] Depth %d: Releasing lock", depth);
    recursive_lock_release(lock);
}

static void recursive_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    recursive_lock_init(&my_recursive_lock);

    while (1) {
        LOG_INF("[RecursiveThread] Starting recursive operation");
        recursive_operation(3, &my_recursive_lock);
        LOG_INF("[RecursiveThread] Recursive operation complete");

        k_sleep(K_SECONDS(1));
    }
}

/*
 * 中断中释放信号量
 */
static void timer_expiry_handler(struct k_timer *timer)
{
    ARG_UNUSED(timer);

    /* 在中断上下文释放信号量 */
    k_sem_give(&binary_sem);
    LOG_DBG("[ISR] Semaphore given from timer interrupt");
}

static K_TIMER_DEFINE(my_timer, timer_expiry_handler, NULL);

/* 线程栈定义 */
K_THREAD_STACK_DEFINE(binary_sem_stack, 2048);
K_THREAD_STACK_DEFINE(producer_stack, 2048);
K_THREAD_STACK_DEFINE(consumer_stack, 2048);
K_THREAD_STACK_DEFINE(mutex_a_stack, 2048);
K_THREAD_STACK_DEFINE(mutex_b_stack, 2048);
K_THREAD_STACK_DEFINE(recursive_stack, 2048);

static struct k_thread binary_sem_thread_data;
static struct k_thread producer_thread_data;
static struct k_thread consumer_thread_data;
static struct k_thread mutex_a_thread_data;
static struct k_thread mutex_b_thread_data;
static struct k_thread recursive_thread_data;

int main(void)
{
    printk("\n=== Zephyr RTOS Semaphore Demo ===\n\n");

    /* 创建线程 */
    k_thread_create(&binary_sem_thread_data, binary_sem_stack,
                    K_THREAD_STACK_SIZEOF(binary_sem_stack),
                    binary_sem_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);

    k_thread_create(&producer_thread_data, producer_stack,
                    K_THREAD_STACK_SIZEOF(producer_stack),
                    counting_producer, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);

    k_thread_create(&consumer_thread_data, consumer_stack,
                    K_THREAD_STACK_SIZEOF(consumer_stack),
                    counting_consumer, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);

    k_thread_create(&mutex_a_thread_data, mutex_a_stack,
                    K_THREAD_STACK_SIZEOF(mutex_a_stack),
                    mutex_thread_a, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);

    k_thread_create(&mutex_b_thread_data, mutex_b_stack,
                    K_THREAD_STACK_SIZEOF(mutex_b_stack),
                    mutex_thread_b, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);

    k_thread_create(&recursive_thread_data, recursive_stack,
                    K_THREAD_STACK_SIZEOF(recursive_stack),
                    recursive_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);

    /* 启动定时器 - 周期性释放信号量 */
    k_timer_start(&my_timer, K_MSEC(500), K_MSEC(500));

    /* 主循环 */
    while (1) {
        k_sleep(K_SECONDS(10));

        /* 打印统计信息 */
        LOG_INF("[Main] Binary sem count: %u", k_sem_count_get(&binary_sem));
        LOG_INF("[Main] Counting sem count: %u", k_sem_count_get(&counting_sem));
        LOG_INF("[Main] Shared resource: %u", shared_resource);
    }

    return 0;
}
