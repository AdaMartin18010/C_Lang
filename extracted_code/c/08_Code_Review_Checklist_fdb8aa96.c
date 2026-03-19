/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 440
 * Language: c
 * Block ID: fdb8aa96
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例：死锁（锁顺序不一致）
pthread_mutex_t lock_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_b = PTHREAD_MUTEX_INITIALIZER;

void thread1_work(void) {
    pthread_mutex_lock(&lock_a);
    pthread_mutex_lock(&lock_b);  // 可能死锁
    // 操作共享资源
    pthread_mutex_unlock(&lock_b);
    pthread_mutex_unlock(&lock_a);
}

void thread2_work(void) {
    pthread_mutex_lock(&lock_b);  // 与thread1顺序相反！
    pthread_mutex_lock(&lock_a);  // 可能死锁
    // 操作共享资源
    pthread_mutex_unlock(&lock_a);
    pthread_mutex_unlock(&lock_b);
}

// ✅ 正确示例：全局锁顺序
// 规则: 总是先A后B

void thread1_work_safe(void) {
    pthread_mutex_lock(&lock_a);  // 先A
    pthread_mutex_lock(&lock_b);  // 后B
    // 操作共享资源
    pthread_mutex_unlock(&lock_b);
    pthread_mutex_unlock(&lock_a);
}

void thread2_work_safe(void) {
    pthread_mutex_lock(&lock_a);  // 同样先A
    pthread_mutex_lock(&lock_b);  // 后B
    // 操作共享资源
    pthread_mutex_unlock(&lock_b);
    pthread_mutex_unlock(&lock_a);
}

// ✅ 更好示例：使用层级锁（自动检查顺序）
#define LOCK_LEVEL_A 10
#define LOCK_LEVEL_B 20

struct hierarchical_lock {
    pthread_mutex_t mutex;
    int level;
};

static __thread int current_lock_level = 0;

void hierarchical_lock_acquire(struct hierarchical_lock *hl) {
    if (hl->level <= current_lock_level) {
        fprintf(stderr, "锁顺序违规: 尝试获取层级%d的锁，当前层级%d\n",
                hl->level, current_lock_level);
        abort();
    }
    pthread_mutex_lock(&hl->mutex);
    current_lock_level = hl->level;
}

void hierarchical_lock_release(struct hierarchical_lock *hl) {
    current_lock_level = 0;  // 简化处理，实际应恢复之前层级
    pthread_mutex_unlock(&hl->mutex);
}
