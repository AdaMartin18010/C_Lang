/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\02_Synchronization_Matrix.md
 * Line: 41
 * Language: c
 * Block ID: 01e96f54
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// sync_primitives_overview.c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * 同步原语概览:
 *
 * 1. 互斥锁 (Mutex)
 *    - 用途: 互斥访问临界区
 *    - 特性: 阻塞等待，上下文切换开销
 *    - 适用: 临界区执行时间较长
 *
 * 2. 读写锁 (RWLock)
 *    - 用途: 读多写少的场景
 *    - 特性: 读共享，写独占
 *    - 适用: 读操作远多于写操作
 *
 * 3. 自旋锁 (Spinlock)
 *    - 用途: 短暂临界区保护
 *    - 特性: 忙等待，无上下文切换
 *    - 适用: 多核CPU，临界区极短
 *
 * 4. 信号量 (Semaphore)
 *    - 用途: 资源计数和访问控制
 *    - 特性: 可多个线程同时访问
 *    - 适用: 连接池、资源池等
 *
 * 5. 条件变量 (Condition Variable)
 *    - 用途: 线程间事件通知
 *    - 特性: 必须与互斥锁配合使用
 *    - 适用: 生产者-消费者模式
 */

int main(void) {
    printf("POSIX线程同步原语:\n");
    printf("- pthread_mutex_t: 互斥锁\n");
    printf("- pthread_rwlock_t: 读写锁\n");
    printf("- pthread_spinlock_t: 自旋锁\n");
    printf("- sem_t: 信号量\n");
    printf("- pthread_cond_t: 条件变量\n");
    return 0;
}
