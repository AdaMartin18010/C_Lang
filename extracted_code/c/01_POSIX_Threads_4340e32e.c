/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 1772
 * Language: c
 * Block ID: 4340e32e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 陷阱6：高优先级线程被低优先级线程阻塞 */
/*
 * 场景：
 * - T_low: 低优先级，持有锁M
 * - T_mid: 中优先级，不相关计算
 * - T_high: 高优先级，需要锁M
 *
 * T_low持有M，被T_mid抢占，T_high等待M
 * 结果：T_high被T_mid间接阻塞（优先级反转）
 */

/* Linux解决方案：优先级继承协议 */
void enable_priority_inheritance(pthread_mutex_t *mutex) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
    pthread_mutex_init(mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

/* 或者使用优先级天花板协议 */
void enable_priority_ceiling(pthread_mutex_t *mutex, int ceiling) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_PROTECT);
    pthread_mutexattr_setprioceiling(&attr, ceiling);
    pthread_mutex_init(mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}
