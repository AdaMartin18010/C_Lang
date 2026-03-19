/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\05_Concurrency_Debug.md
 * Line: 99
 * Language: c
 * Block ID: ee2511c9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 竞态条件: 非原子操作
int counter = 0;
// Thread A & B:
counter++;  // 读取-修改-写入,非原子

// ✅ 解决方案1: 原子操作
_Atomic int counter = 0;
counter++;

// ✅ 解决方案2: 互斥锁
pthread_mutex_lock(&m);
counter++;
pthread_mutex_unlock(&m);

// ✅ 解决方案3: C11原子
atomic_fetch_add(&counter, 1, memory_order_relaxed);
