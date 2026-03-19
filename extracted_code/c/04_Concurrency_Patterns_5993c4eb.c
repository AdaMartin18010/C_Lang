/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\04_Concurrency_Patterns.md
 * Line: 315
 * Language: c
 * Block ID: 5993c4eb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 死锁场景
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;

// 线程A               线程B
pthread_mutex_lock(&m1);    pthread_mutex_lock(&m2);
pthread_mutex_lock(&m2);    pthread_mutex_lock(&m1);  // 死锁！
// ...                  // ...
pthread_mutex_unlock(&m2);  pthread_mutex_unlock(&m1);
pthread_mutex_unlock(&m1);  pthread_mutex_unlock(&m2);

// 解决方案: 统一获取顺序
// 总是先m1后m2
