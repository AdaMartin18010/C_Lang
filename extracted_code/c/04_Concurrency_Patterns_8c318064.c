/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\04_Concurrency_Patterns.md
 * Line: 906
 * Language: c
 * Block ID: 8c318064
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 1. 减少锁持有时间
pthread_mutex_lock(&mutex);
int local_copy = shared_data;  // 快速复制
pthread_mutex_unlock(&mutex);
process(local_copy);  // 长时间处理在锁外

// 2. 使用读写锁优化读多写少
pthread_rwlock_rdlock(&rwlock);  // 多个读者
read_data();
pthread_rwlock_unlock(&rwlock);

// 3. 避免在持有锁时调用外部代码
pthread_mutex_lock(&mutex);
// 不要在这里调用可能阻塞的函数
pthread_mutex_unlock(&mutex);

// 4. 使用原子操作替代锁 (简单场景)
atomic_fetch_add(&counter, 1);  // 比锁更高效
