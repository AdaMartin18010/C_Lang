/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\02_Synchronization_Matrix.md
 * Line: 1106
 * Language: c
 * Block ID: 361b1ef5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// deadlock_prevention.c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * 死锁产生的四个必要条件:
 * 1. 互斥条件: 资源独占
 * 2. 请求与保持: 持有资源同时请求新资源
 * 3. 不剥夺条件: 资源只能主动释放
 * 4. 循环等待: 形成等待环路
 *
 * 预防策略: 破坏其中一个条件
 */

// 策略1: 锁排序 - 全局统一的加锁顺序
pthread_mutex_t lock_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_b = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_c = PTHREAD_MUTEX_INITIALIZER;

// 为每个锁分配唯一ID
#define LOCK_ID_A 1
#define LOCK_ID_B 2
#define LOCK_ID_C 3

// 有序加锁函数
void ordered_lock(pthread_mutex_t *lock1, int id1,
                  pthread_mutex_t *lock2, int id2) {
    if (id1 < id2) {
        pthread_mutex_lock(lock1);
        pthread_mutex_lock(lock2);
    } else {
        pthread_mutex_lock(lock2);
        pthread_mutex_lock(lock1);
    }
}

void ordered_unlock(pthread_mutex_t *lock1, pthread_mutex_t *lock2) {
    pthread_mutex_unlock(lock1);
    pthread_mutex_unlock(lock2);
}

// 策略2: 超时机制
trylock_with_timeout(pthread_mutex_t *lock1, pthread_mutex_t *lock2) {
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 1;  // 1秒超时

    if (pthread_mutex_timedlock(lock1, &timeout) != 0) {
        return -1;  // 超时
    }

    if (pthread_mutex_timedlock(lock2, &timeout) != 0) {
        pthread_mutex_unlock(lock1);  // 释放已获取的锁
        return -1;
    }

    return 0;
}

// 策略3: 一次性获取所有锁 (银行家算法简化)
typedef struct {
    pthread_mutex_t *locks[3];
    int count;
} lock_set_t;

int acquire_all_or_none(lock_set_t *set) {
    int acquired = 0;

    // 尝试获取所有锁
    for (int i = 0; i < set->count; i++) {
        if (pthread_mutex_trylock(set->locks[i]) != 0) {
            // 获取失败，释放已获取的锁
            for (int j = 0; j < acquired; j++) {
                pthread_mutex_unlock(set->locks[j]);
            }
            return -1;
        }
        acquired++;
    }

    return 0;
}

// 策略4: 使用层级锁
#define LOCK_LEVEL_HIGH  10
#define LOCK_LEVEL_MED   20
#define LOCK_LEVEL_LOW   30

typedef struct {
    pthread_mutex_t mutex;
    int level;
} hierarchical_lock_t;

int hierarchical_lock(hierarchical_lock_t *current, hierarchical_lock_t *requested) {
    // 只能获取同级或更低级的锁
    if (requested->level < current->level) {
        fprintf(stderr, "Error: Cannot acquire higher level lock\n");
        return -1;
    }
    pthread_mutex_lock(&requested->mutex);
    return 0;
}

// 检测死锁 (运行时检查)
typedef struct {
    pthread_mutex_t *owned_locks[10];
    int count;
    pthread_t thread_id;
} lock_tracker_t;

_Thread_local lock_tracker_t tracker = {.count = 0};

void tracked_lock(pthread_mutex_t *lock) {
    // 检查是否会造成循环等待
    for (int i = 0; i < tracker.count; i++) {
        if (tracker.owned_locks[i] == lock) {
            fprintf(stderr, "Warning: Attempting to relock owned mutex\n");
        }
    }

    pthread_mutex_lock(lock);
    tracker.owned_locks[tracker.count++] = lock;
}

void tracked_unlock(pthread_mutex_t *lock) {
    pthread_mutex_unlock(lock);

    // 从追踪列表移除
    for (int i = 0; i < tracker.count; i++) {
        if (tracker.owned_locks[i] == lock) {
            tracker.owned_locks[i] = tracker.owned_locks[--tracker.count];
            break;
        }
    }
}

int main(void) {
    printf("=== 死锁预防策略 ===\n");
    printf("1. 锁排序: 全局统一的加锁顺序\n");
    printf("2. 超时机制: 避免无限等待\n");
    printf("3. 一次性获取: 要么全获取，要么全不获取\n");
    printf("4. 层级锁: 限制锁的获取顺序\n");
    printf("5. 运行时检测: 追踪锁的持有情况\n");
    return 0;
}
