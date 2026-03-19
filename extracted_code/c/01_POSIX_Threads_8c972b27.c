/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 1599
 * Language: c
 * Block ID: 8c972b27
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 陷阱3：同线程重复加锁导致死锁 */
pthread_mutex_t normal_mutex = PTHREAD_MUTEX_INITIALIZER;

void helper_function() {
    pthread_mutex_lock(&normal_mutex);  // 死锁！已经持有
    // ... 做一些事情
    pthread_mutex_unlock(&normal_mutex);
}

void main_function() {
    pthread_mutex_lock(&normal_mutex);
    helper_function();  // 尝试再次加锁
    pthread_mutex_unlock(&normal_mutex);
}

/* 修复方案1：使用递归锁 */
pthread_mutex_t recursive_mutex;

void init_recursive_mutex() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&recursive_mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

/* 修复方案2：重构代码避免递归加锁 */
void helper_function_internal() {
    // 假设锁已持有，不做加锁操作
}

void main_function_fixed() {
    pthread_mutex_lock(&normal_mutex);
    helper_function_internal();
    pthread_mutex_unlock(&normal_mutex);
}
