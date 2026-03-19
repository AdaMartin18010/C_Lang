/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\05_Concurrency_Debug.md
 * Line: 58
 * Language: c
 * Block ID: bca80617
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 死锁模式1: 加锁顺序不一致
Thread A: lock(m1); lock(m2);
Thread B: lock(m2); lock(m1);

// ✅ 解决方案: 统一加锁顺序
// 所有线程按相同顺序加锁: m1 -> m2

// ❌ 死锁模式2: 自锁
pthread_mutex_lock(&m);
// ... 同一线程再次加锁 ...
pthread_mutex_lock(&m);  // 死锁!

// ✅ 解决方案: 使用递归锁
pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
