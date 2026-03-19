/*
 * Auto-generated from: 12_Practice_Exercises\08_Concurrency_Parallelism_Labs.md
 * Line: 277
 * Language: c
 * Block ID: ff7d548e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 测试不同内存序的影响

typedef struct {
    atomic_int x;
    atomic_int y;
    atomic_int z;
} SharedData;

// 测试store/load的内存序
void test_memory_order(void);

// 实现seqlock（顺序锁）
typedef struct {
    atomic_ulong sequence;
    // 数据...
} SeqLock;

void seqlock_read_lock(SeqLock *sl);
bool seqlock_read_retry(SeqLock *sl, unsigned long start);
void seqlock_write_lock(SeqLock *sl);
void seqlock_write_unlock(SeqLock *sl);
