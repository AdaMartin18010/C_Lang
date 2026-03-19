/*
 * Auto-generated from: 03_System_Technology_Domains\14_Concurrency_Parallelism\01_POSIX_Threads.md
 * Line: 1806
 * Language: c
 * Block ID: 55ace4c5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 陷阱7：错误路径忘记解锁 */
pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int *data;
    size_t size;
} dataset_t;

int process_data(dataset_t *ds, int index) {
    pthread_mutex_lock(&data_mutex);

    if (index < 0 || index >= ds->size) {
        return -1;  // BUG: mutex未解锁！
    }

    if (ds->data == NULL) {
        return -2;  // BUG: mutex未解锁！
    }

    int value = ds->data[index];
    // 处理...

    pthread_mutex_unlock(&data_mutex);
    return value;
}

/* 修复方案1：使用goto清理 */
int process_data_fixed(dataset_t *ds, int index) {
    pthread_mutex_lock(&data_mutex);

    int ret = -1;
    if (index < 0 || index >= ds->size) {
        ret = -1;
        goto cleanup;
    }

    if (ds->data == NULL) {
        ret = -2;
        goto cleanup;
    }

    ret = ds->data[index];

cleanup:
    pthread_mutex_unlock(&data_mutex);
    return ret;
}

/* 修复方案2：使用包装函数和宏 */
#define LOCK_GUARD(mutex) \
    for (int _locked = 1; _locked; _locked = 0, pthread_mutex_unlock(&(mutex)))

int process_data_clean(dataset_t *ds, int index) {
    int ret = -1;
    LOCK_GUARD(data_mutex) {
        if (index < 0 || index >= ds->size) {
            ret = -1;
            continue;  // 退出循环，自动解锁
        }
        ret = ds->data[index];
    }
    return ret;
}

/* 修复方案3：使用pthread_cleanup_push */
int process_data_cleanup(dataset_t *ds, int index) {
    pthread_mutex_lock(&data_mutex);
    pthread_cleanup_push(pthread_mutex_unlock, &data_mutex);

    if (index < 0 || index >= ds->size) {
        pthread_cleanup_pop(1);  // 执行清理（解锁）
        return -1;
    }

    int value = ds->data[index];

    pthread_cleanup_pop(1);  // 执行清理
    return value;
}
