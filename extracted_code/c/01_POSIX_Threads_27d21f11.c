/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 2565
 * Language: c
 * Block ID: 27d21f11
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*========================================
 * 粗粒度锁（低并发，简单）
 *========================================*/
typedef struct {
    pthread_mutex_t lock;  // 保护整个结构
    int field_a;
    int field_b;
    int field_c;
} coarse_grained_t;

/*========================================
 * 细粒度锁（高并发，复杂）
 *========================================*/
typedef struct {
    pthread_mutex_t lock_a;  // 独立保护每个字段
    pthread_mutex_t lock_b;
    pthread_mutex_t lock_c;
    int field_a;
    int field_b;
    int field_c;
} fine_grained_t;

/*========================================
 * 锁分段（中等粒度，折中）
 *========================================*/
#define NUM_SEGMENTS 16

typedef struct {
    pthread_mutex_t locks[NUM_SEGMENTS];
    int data[NUM_SEGMENTS][100];  // 分段数据
} striped_lock_t;

void striped_write(striped_lock_t *sl, int index, int value) {
    int segment = index % NUM_SEGMENTS;
    pthread_mutex_lock(&sl->locks[segment]);
    sl->data[segment][index / NUM_SEGMENTS] = value;
    pthread_mutex_unlock(&sl->locks[segment]);
}

/*========================================
 * 读写锁优化读多写少场景
 *========================================*/
typedef struct {
    pthread_rwlock_t rwlock;
    int config_data[1000];  // 配置数据，很少修改
} config_t;

int read_config(config_t *cfg, int index) {
    pthread_rwlock_rdlock(&cfg->rwlock);
    int value = cfg->config_data[index];
    pthread_rwlock_unlock(&cfg->rwlock);
    return value;
}

void update_config(config_t *cfg, int index, int value) {
    pthread_rwlock_wrlock(&cfg->rwlock);
    cfg->config_data[index] = value;
    pthread_rwlock_unlock(&cfg->rwlock);
}
